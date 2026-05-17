#include "server/HttpServer.hpp"
#include "logging/WebLogger.hpp"
#include <sstream>

static WebLogger logger{"HttpServer"};

HttpServer::HttpServer(ModuleManager &modules,
                       EventBus &eventBus,
                       Scheduler &scheduler,
                       MQTTClient &mqtt)
    : _modules(modules),
      _eventBus(eventBus),
      _scheduler(scheduler),
      _mqtt(mqtt) {
}

void HttpServer::start(int port) {
    if (_running) return;

    _running = true;
    _startTime = std::chrono::steady_clock::now();

    _server.set_pre_routing_handler([](const httplib::Request& req, httplib::Response& res) {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, PATCH, DELETE, OPTIONS");
            res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");

            if (req.method == "OPTIONS") {
                res.status = 204;
                return httplib::Server::HandlerResponse::Handled;
            }
            return httplib::Server::HandlerResponse::Unhandled;
        });

    registerRoutes();
    bindRoutes();

    logger.info("Registered " + std::to_string(_routes.size()) + " endpoint(s):");
    for (const auto& ep : _routes) {
        std::string method;
        switch (ep.getMethod()) {
            case HttpMethod::GET:   method = "GET   "; break;
            case HttpMethod::POST:  method = "POST  "; break;
            case HttpMethod::PUT:   method = "PUT   "; break;
            case HttpMethod::PATCH: method = "PATCH "; break;
            default:                method = "?     "; break;
        }
        logger.info("  " + method + ep.getPath());
    }

    _server.set_logger([](const auto &req, const auto &res) {
        const std::string msg = req.method + " " + req.path + " -> " + std::to_string(res.status);
        if (res.status >= 500)
            WebLogger::emit("error", "HttpServer", msg);
        else if (res.status >= 400)
            WebLogger::emit("warn", "HttpServer", msg);
        else
            WebLogger::emit("info", "HttpServer", msg);
    });

    _serverThread = std::thread([this, port]() {
        logger.info("Starting on port " + std::to_string(port));
        _server.listen("0.0.0.0", port);
        logger.info("Server stopped");
    });
}

void HttpServer::stop() {
    if (!_running) return;

    logger.info("Stopping...");
    _running = false;
    _server.stop();

    if (_serverThread.joinable())
        _serverThread.join();

    logger.info("Stopped");
}

// ── Routes ────────────────────────────────────────────────────────────────────

void HttpServer::registerRoutes() {
    // Prepend builtin routes before any routes already added via use()
    std::vector<Endpoint> builtins;

    auto add = [&builtins](Endpoint ep) {
        builtins.push_back(std::move(ep));
    };

    add(Endpoint("/api/status", "status", HttpMethod::GET,
        [this](const httplib::Request&, httplib::Response& res) {
            res.set_content(toJson(true, "OK"), "application/json");
        })
        .describe("Check whether the server is running"));

    add(Endpoint("/api/health", "health", HttpMethod::GET,
        [this](const httplib::Request&, httplib::Response& res) {
            long long uptime = uptimeSeconds();
            std::stringstream ss;
            ss << "{"
               << "\"status\":\"ok\","
               << "\"uptime_seconds\":" << uptime << ","
               << "\"uptime_human\":\""
               << (uptime / 3600) << "h "
               << (uptime % 3600 / 60) << "m "
               << (uptime % 60) << "s\","
               << "\"mqtt_connected\":" << (_mqtt.isConnected() ? "true" : "false") << ","
               << "\"modules_loaded\":" << _modules.getModules().size()
               << "}";
            res.set_content(ss.str(), "application/json");
        })
        .describe("Return uptime, MQTT connection status and number of loaded modules"));

    add(Endpoint("/api/metrics", "metrics", HttpMethod::GET,
        [this](const httplib::Request&, httplib::Response& res) {
            long long uptime = uptimeSeconds();
            long vmRss = 0;
            if (FILE* f = fopen("/proc/self/status", "r")) {
                char line[128];
                while (fgets(line, sizeof(line), f))
                    if (strncmp(line, "VmRSS:", 6) == 0) { sscanf(line + 6, " %ld", &vmRss); break; }
                fclose(f);
            }
            std::stringstream ss;
            ss << "{"
               << "\"uptime_seconds\":" << uptime << ","
               << "\"mqtt_connected\":" << (_mqtt.isConnected() ? "true" : "false") << ","
               << "\"modules_loaded\":" << _modules.getModules().size() << ","
               << "\"memory_rss_kb\":" << vmRss << ","
               << "\"endpoints_registered\":" << _routes.size()
               << "}";
            res.set_content(ss.str(), "application/json");
        })
        .describe("Return detailed runtime metrics including memory usage and endpoint count"));

    // Insert builtins at the front so controller routes come after
    _routes.insert(_routes.begin(),
                   std::make_move_iterator(builtins.begin()),
                   std::make_move_iterator(builtins.end()));
}

void HttpServer::use(IController& controller) {
    controller.registerRoutes(*this);
}

void HttpServer::addRoute(Endpoint ep) {
    _routes.push_back(std::move(ep));
}

void HttpServer::bindRoutes() {
    for (auto &endpoint: _routes) {
        switch (endpoint.getMethod()) {
            case HttpMethod::GET:
                _server.Get(endpoint.getPath(), [&endpoint](const httplib::Request &req, httplib::Response &res) {
                    endpoint.handle(req, res);
                });
                break;
            case HttpMethod::POST:
                _server.Post(endpoint.getPath(), [&endpoint](const httplib::Request &req, httplib::Response &res) {
                    endpoint.handle(req, res);
                });
                break;
            case HttpMethod::PUT:
                _server.Put(endpoint.getPath(), [&endpoint](const httplib::Request &req, httplib::Response &res) {
                    endpoint.handle(req, res);
                });
                break;
            case HttpMethod::PATCH:
                _server.Patch(endpoint.getPath(), [&endpoint](const httplib::Request &req, httplib::Response &res) {
                    endpoint.handle(req, res);
                });
                break;
            case HttpMethod::DELETE:
                _server.Delete(endpoint.getPath(), [&endpoint](const httplib::Request &req, httplib::Response &res) {
                    endpoint.handle(req, res);
                });
                break;
            default:
                logger.warn("Unsupported method for endpoint: " + endpoint.toString());
                break;
        }
    }
}

// ── Helpers ───────────────────────────────────────────────────────────────────

std::string HttpServer::toJson(bool success, const std::string &output) {
    std::stringstream ss;
    ss << "{"
            << "\"success\": " << (success ? "true" : "false") << ","
            << "\"output\": \"" << output << "\""
            << "}";
    return ss.str();
}

long long HttpServer::uptimeSeconds() const {
    return std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - _startTime
    ).count();
}
