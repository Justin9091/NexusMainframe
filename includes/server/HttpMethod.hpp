// HttpMethod.hpp

#ifndef NEXUSCORE_HTTPMETHOD_HPP
#define NEXUSCORE_HTTPMETHOD_HPP

#include <string>
#include <stdexcept>

enum class HttpMethod {
    GET,
    POST,
    PATCH,
    PUT,
    HEAD,
    OPTIONS
};

inline std::string httpMethodToString(HttpMethod method) {
    switch (method) {
        case HttpMethod::GET:     return "GET";
        case HttpMethod::POST:    return "POST";
        case HttpMethod::PUT:     return "PUT";
        case HttpMethod::PATCH:   return "PATCH";
        case HttpMethod::HEAD:    return "HEAD";
        case HttpMethod::OPTIONS: return "OPTIONS";
        default: throw std::invalid_argument("Unknown HttpMethod");
    }
}

#endif //NEXUSCORE_HTTPMETHOD_HPP