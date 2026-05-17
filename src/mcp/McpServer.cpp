#include "mcp/McpServer.hpp"
#include <iostream>
#include <sstream>

McpServer::McpServer(const std::vector<Endpoint>& routes, std::string apiBase)
    : _routes(routes), _apiBase(std::move(apiBase)) {}

// ── Lifecycle ─────────────────────────────────────────────────────────────────

void McpServer::start(int port) {
    _running = true;

    _srv.set_pre_routing_handler([](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        return httplib::Server::HandlerResponse::Unhandled;
    });

    _srv.Post("/mcp", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto response = dispatch(nlohmann::json::parse(req.body));
            if (response.is_null()) {
                res.status = 204; // notification — no response body
                return;
            }
            res.set_content(response.dump(), "application/json");
        } catch (...) {
            res.status = 400;
            res.set_content(err(nullptr, -32700, "Parse error").dump(), "application/json");
        }
    });

    _srv.Get("/mcp/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(R"({"status":"ok","server":"nexus-mcp"})", "application/json");
    });

    _thread = std::thread([this, port]() {
        std::cout << "[McpServer] Listening on port " << port << "\n";
        _srv.listen("0.0.0.0", port);
    });
}

void McpServer::stop() {
    if (!_running) return;
    _running = false;
    _srv.stop();
    if (_thread.joinable()) _thread.join();
    std::cout << "[McpServer] Stopped\n";
}

// ── Dispatch ──────────────────────────────────────────────────────────────────

nlohmann::json McpServer::dispatch(const nlohmann::json& req) {
    auto id     = req.value("id", nlohmann::json{});
    auto method = req.value("method", std::string{});
    auto params = req.value("params", nlohmann::json::object());

    if (method == "initialize")             return handleInitialize(id);
    if (method == "notifications/initialized") return nullptr; // notification, no response
    if (method == "ping")                   return ok(id, nlohmann::json::object());
    if (method == "tools/list")             return handleToolsList(id);
    if (method == "tools/call")             return handleToolsCall(params, id);

    return err(id, -32601, "Method not found: " + method);
}

nlohmann::json McpServer::handleInitialize(const nlohmann::json& id) {
    return ok(id, {
        {"protocolVersion", "2025-03-26"},
        {"serverInfo",      {{"name", "nexus-mcp"}, {"version", "1.0.0"}}},
        {"capabilities",    {{"tools", nlohmann::json::object()}}}
    });
}

// ── Tools list — built from Endpoint metadata ─────────────────────────────────

nlohmann::json McpServer::handleToolsList(const nlohmann::json& id) {
    nlohmann::json tools = nlohmann::json::array();

    for (const auto& ep : _routes) {
        nlohmann::json tool = {
            {"name",        ep.getName()},
            {"description", ep.getDescription().empty()
                                ? ep.toString()
                                : ep.getDescription()},
            {"inputSchema", ep.buildInputSchema()}
        };
        tools.push_back(std::move(tool));
    }

    return ok(id, {{"tools", tools}});
}

// ── Tool call — proxy to HTTP API ─────────────────────────────────────────────

nlohmann::json McpServer::handleToolsCall(const nlohmann::json& params, const nlohmann::json& id) {
    std::string name = params.value("name", std::string{});
    auto args        = params.value("arguments", nlohmann::json::object());

    const Endpoint* ep = nullptr;
    for (const auto& route : _routes)
        if (route.getName() == name) { ep = &route; break; }

    if (!ep)
        return err(id, -32602, "Unknown tool: " + name);

    return ok(id, {{"content", {text(callEndpoint(*ep, args).dump(2))}}});
}

// ── HTTP proxy helpers ────────────────────────────────────────────────────────

nlohmann::json McpServer::callEndpoint(const Endpoint& ep, const nlohmann::json& args) {
    std::string url  = buildUrl(ep, args);
    auto body        = buildBody(ep, args);
    std::string bodyStr = body.empty() ? "" : body.dump();

    // Parse host and port out of _apiBase (format: http://host:port)
    std::string host = "localhost";
    int port = 8080;
    {
        auto s = _apiBase;
        if (s.starts_with("http://"))  s = s.substr(7);
        if (s.starts_with("https://")) s = s.substr(8);
        auto colon = s.rfind(':');
        if (colon != std::string::npos) {
            host = s.substr(0, colon);
            port = std::stoi(s.substr(colon + 1));
        } else {
            host = s;
        }
    }

    httplib::Client cli(host, port);
    cli.set_connection_timeout(3);
    cli.set_read_timeout(5);

    httplib::Result res;
    switch (ep.getMethod()) {
        case HttpMethod::GET:
            res = cli.Get(url);
            break;
        case HttpMethod::POST:
            res = cli.Post(url, bodyStr, "application/json");
            break;
        case HttpMethod::PUT:
            res = cli.Put(url, bodyStr, "application/json");
            break;
        case HttpMethod::PATCH:
            res = cli.Patch(url, bodyStr, "application/json");
            break;
        default:
            return {{"error", "Unsupported HTTP method"}};
    }

    if (!res)
        return {{"error", "HTTP request failed: " + httplib::to_string(res.error())}};

    try {
        return nlohmann::json::parse(res->body);
    } catch (...) {
        return {{"raw", res->body}, {"status", res->status}};
    }
}

std::string McpServer::buildUrl(const Endpoint& ep, const nlohmann::json& args) {
    std::string url = ep.getPath();
    std::string query;

    for (const auto& p : ep.getParams()) {
        if (!args.contains(p.name)) continue;

        std::string val = args[p.name].is_string()
                              ? args[p.name].get<std::string>()
                              : args[p.name].dump();

        if (p.in == "path") {
            auto pos = url.find(':' + p.name);
            if (pos != std::string::npos)
                url.replace(pos, p.name.size() + 1, val);
        } else if (p.in == "query") {
            query += (query.empty() ? '?' : '&');
            query += p.name + '=' + val;
        }
    }
    return url + query;
}

nlohmann::json McpServer::buildBody(const Endpoint& ep, const nlohmann::json& args) {
    nlohmann::json body = nlohmann::json::object();
    for (const auto& p : ep.getParams())
        if (p.in == "body" && args.contains(p.name))
            body[p.name] = args[p.name];
    return body;
}

// ── JSON-RPC helpers ──────────────────────────────────────────────────────────

nlohmann::json McpServer::ok(const nlohmann::json& id, nlohmann::json result) {
    return {{"jsonrpc", "2.0"}, {"id", id}, {"result", std::move(result)}};
}

nlohmann::json McpServer::err(const nlohmann::json& id, int code, const std::string& msg) {
    return {{"jsonrpc", "2.0"}, {"id", id}, {"error", {{"code", code}, {"message", msg}}}};
}

nlohmann::json McpServer::text(const std::string& content) {
    return {{"type", "text"}, {"text", content}};
}
