#include "server/HttpServer.hpp"
#include <iostream>
#include <sstream>

HttpServer::HttpServer(ModuleManager& modules,
                       CommandRegistry& commands,
                       EventBus& eventBus,
                       Scheduler& scheduler,
                       MQTTClient& mqtt)
    : _modules(modules),
      _commands(commands),
      _eventBus(eventBus),
      _scheduler(scheduler),
      _mqtt(mqtt) {}

void HttpServer::start(int port) {
    if (_running) return;

    _running   = true;
    _startTime = std::chrono::steady_clock::now();

    registerRoutes();
    bindRoutes();

    _server.set_logger([](const auto& req, const auto& res) {
        std::cout << req.method << " " << req.path
                  << " -> " << res.status << std::endl;
    });

    _serverThread = std::thread([this, port]() {
        std::cout << "[HttpServer] Starting on port " << port << std::endl;
        _server.listen("0.0.0.0", port);
        std::cout << "[HttpServer] Server stopped" << std::endl;
    });
}

void HttpServer::stop() {
    if (!_running) return;

    std::cout << "[HttpServer] Stopping..." << std::endl;
    _running = false;
    _server.stop();

    if (_serverThread.joinable())
        _serverThread.join();

    std::cout << "[HttpServer] Stopped" << std::endl;
}

// ── Routes ────────────────────────────────────────────────────────────────────

void HttpServer::registerRoutes() {
    _routes = {
        {
            "/api/status", "status", HttpMethod::GET,
            [this](const httplib::Request&, httplib::Response& res) {
                res.set_content(toJson(true, "OK"), "application/json");
            }
        },
        {
            "/api/health", "health", HttpMethod::GET,
            [this](const httplib::Request&, httplib::Response& res) {
                long long uptime = uptimeSeconds();

                std::stringstream ss;
                ss << "{"
                   << "\"status\": \"ok\","
                   << "\"uptime_seconds\": " << uptime << ","
                   << "\"uptime_human\": \""
                       << (uptime / 3600)      << "h "
                       << (uptime % 3600 / 60) << "m "
                       << (uptime % 60)        << "s\","
                   << "\"mqtt_connected\": " << (_mqtt.isConnected() ? "true" : "false") << ","
                   << "\"modules_loaded\": " << _modules.getModules().size()
                   << "}";

                res.set_content(ss.str(), "application/json");
            }
        },
        {
            "/api/modules", "modules", HttpMethod::GET,
            [this](const httplib::Request&, httplib::Response& res) {
                auto modules = _modules.getModules();

                std::string out;
                for (auto& m : modules)
                    out += m.name + ",";

                res.set_content(toJson(true, out), "application/json");
            }
        },
        {
            "/api/command", "command", HttpMethod::POST,
            [this](const httplib::Request& req, httplib::Response& res) {
                try {
                    std::string cmd = req.has_param("cmd")
                        ? req.get_param_value("cmd")
                        : req.body;

                    // auto result = _commands.execute(cmd);
                    res.set_content(toJson(true, "idk"), "application/json");
                }
                catch (const std::exception& e) {
                    res.status = 500;
                    res.set_content(toJson(false, e.what()), "application/json");
                }
            }
        },
        {
            "/api/metrics", "metrics", HttpMethod::GET,
            [this](const httplib::Request&, httplib::Response& res) {

            }
        }
    };
}

void HttpServer::bindRoutes() {
    for (auto& endpoint : _routes) {
        switch (endpoint.getMethod()) {
            case HttpMethod::GET:
                _server.Get(endpoint.getPath(),    [&endpoint](const httplib::Request& req, httplib::Response& res) { endpoint.handle(req, res); });
                break;
            case HttpMethod::POST:
                _server.Post(endpoint.getPath(),   [&endpoint](const httplib::Request& req, httplib::Response& res) { endpoint.handle(req, res); });
                break;
            case HttpMethod::PUT:
                _server.Put(endpoint.getPath(),    [&endpoint](const httplib::Request& req, httplib::Response& res) { endpoint.handle(req, res); });
                break;
            case HttpMethod::PATCH:
                _server.Patch(endpoint.getPath(),  [&endpoint](const httplib::Request& req, httplib::Response& res) { endpoint.handle(req, res); });
                break;
            default:
                std::cerr << "[HttpServer] Unsupported method: " << endpoint.toString() << std::endl;
                break;
        }
    }
}

// ── Helpers ───────────────────────────────────────────────────────────────────

std::string HttpServer::toJson(bool success, const std::string& output) {
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