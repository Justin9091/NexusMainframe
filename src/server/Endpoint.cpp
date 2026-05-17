#include "server/Endpoint.hpp"

Endpoint::Endpoint(std::string path, std::string name, HttpMethod method, Handler handler)
    : _path(std::move(path)),
      _name(std::move(name)),
      _method(method),
      _handler(std::move(handler))
{
    if (_path.empty())  throw std::invalid_argument("Endpoint path must not be empty.");
    if (_name.empty())  throw std::invalid_argument("Endpoint name must not be empty.");
    if (!_handler)      throw std::invalid_argument("Endpoint handler must not be null.");
}

Endpoint& Endpoint::describe(std::string description) {
    _description = std::move(description);
    return *this;
}

Endpoint& Endpoint::param(std::string name, std::string in, std::string type,
                           std::string description, bool required) {
    _params.push_back({
        std::move(name),
        std::move(in),
        std::move(type),
        std::move(description),
        required
    });
    return *this;
}

const std::string&               Endpoint::getPath()        const noexcept { return _path;        }
const std::string&               Endpoint::getName()        const noexcept { return _name;        }
HttpMethod                        Endpoint::getMethod()      const noexcept { return _method;      }
const Endpoint::Handler&          Endpoint::getHandler()     const noexcept { return _handler;     }
const std::string&               Endpoint::getDescription() const noexcept { return _description; }
const std::vector<EndpointParam>& Endpoint::getParams()     const noexcept { return _params;      }

nlohmann::json Endpoint::buildInputSchema() const {
    nlohmann::json props = nlohmann::json::object();
    nlohmann::json required = nlohmann::json::array();

    for (const auto& p : _params) {
        nlohmann::json prop = {{"type", p.type}};
        if (!p.description.empty())
            prop["description"] = p.description;
        props[p.name] = prop;
        if (p.required)
            required.push_back(p.name);
    }

    nlohmann::json schema = {{"type", "object"}, {"properties", props}};
    if (!required.empty())
        schema["required"] = required;
    return schema;
}

void Endpoint::handle(const httplib::Request& req, httplib::Response& res) const {
    _handler(req, res);
}

std::string Endpoint::toString() const {
    return "[" + _name + "] " + httpMethodToString(_method) + " " + _path;
}

bool Endpoint::operator==(const Endpoint& other) const noexcept {
    return _method == other._method && _path == other._path;
}

bool Endpoint::operator!=(const Endpoint& other) const noexcept {
    return !(*this == other);
}
