#ifndef NEXUSCORE_ENDPOINT_HPP
#define NEXUSCORE_ENDPOINT_HPP

#include <string>
#include <vector>
#include <functional>
#include <stdexcept>

#include <httplib.h>
#include <nlohmann/json.hpp>

#include "HttpMethod.hpp"

/**
 * @brief Describes a single parameter accepted by an endpoint.
 *
 * Used by Endpoint::buildInputSchema() to produce JSON Schema for MCP.
 */
struct EndpointParam {
    std::string name;        ///< Parameter name.
    std::string in;          ///< Location: @c "path", @c "query", or @c "body".
    std::string type;        ///< JSON type: @c "string", @c "number", @c "boolean", @c "object", @c "array".
    std::string description; ///< Human-readable description.
    bool        required = true; ///< Whether the parameter is required.
};

/**
 * @brief Describes a single HTTP endpoint, including metadata for MCP tool exposure.
 *
 * Endpoints are built with a fluent API and stored in HttpServer.  McpServer
 * reads them to automatically expose REST routes as MCP tools.
 *
 * @code
 * Endpoint ep("/api/devices", "devices.index", HttpMethod::GET, handler);
 * ep.describe("List all registered devices")
 *   .param("page", "query", "number", "Page number", false);
 * server.addRoute(std::move(ep));
 * @endcode
 */
class Endpoint {
public:
    using Handler = std::function<void(const httplib::Request&, httplib::Response&)>; ///< Handler type.

    Endpoint() = default;

    /**
     * @param path    URL path pattern (e.g. @c "/api/devices/:id").
     * @param name    Stable machine-readable identifier (e.g. @c "devices.show").
     * @param method  HTTP method.
     * @param handler Request handler callable.
     */
    Endpoint(std::string path, std::string name, HttpMethod method, Handler handler);

    /**
     * @brief Sets the human-readable description (fluent builder).
     * @param description Description string.
     * @return @c *this for chaining.
     */
    Endpoint& describe(std::string description);

    /**
     * @brief Registers a parameter descriptor (fluent builder).
     * @param name        Parameter name.
     * @param in          Location: @c "path", @c "query", or @c "body".
     * @param type        JSON type string.
     * @param description Human-readable description.
     * @param required    Whether the parameter is mandatory (default @c true).
     * @return @c *this for chaining.
     */
    Endpoint& param(std::string name, std::string in, std::string type,
                    std::string description, bool required = true);

    [[nodiscard]] const std::string&               getPath()        const noexcept;
    [[nodiscard]] const std::string&               getName()        const noexcept;
    [[nodiscard]] HttpMethod                        getMethod()      const noexcept;
    [[nodiscard]] const Handler&                    getHandler()     const noexcept;
    [[nodiscard]] const std::string&               getDescription() const noexcept;
    [[nodiscard]] const std::vector<EndpointParam>& getParams()     const noexcept;

    /**
     * @brief Builds a JSON Schema @c properties object from the registered params.
     * Consumed by McpServer when advertising this endpoint as an MCP tool.
     */
    [[nodiscard]] nlohmann::json buildInputSchema() const;

    /** @brief Dispatches the request to the registered handler. */
    void handle(const httplib::Request& req, httplib::Response& res) const;

    /** @brief Returns a concise string representation for logging (method + path). */
    [[nodiscard]] std::string toString() const;

    bool operator==(const Endpoint& other) const noexcept;
    bool operator!=(const Endpoint& other) const noexcept;

private:
    std::string               _path;
    std::string               _name;
    HttpMethod                _method{};
    Handler                   _handler;
    std::string               _description;
    std::vector<EndpointParam> _params;
};

#endif //NEXUSCORE_ENDPOINT_HPP
