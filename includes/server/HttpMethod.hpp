// HttpMethod.hpp

#ifndef NEXUSCORE_HTTPMETHOD_HPP
#define NEXUSCORE_HTTPMETHOD_HPP

#include <string>
#include <stdexcept>

// Windows defines DELETE as a macro in <windows.h>; undefine before our enum
#ifdef DELETE
#undef DELETE
#endif

enum class HttpMethod {
    GET,
    POST,
    PATCH,
    PUT,
    DELETE,
    HEAD,
    OPTIONS
};

inline std::string httpMethodToString(HttpMethod method) {
    switch (method) {
        case HttpMethod::GET:     return "GET";
        case HttpMethod::POST:    return "POST";
        case HttpMethod::PUT:     return "PUT";
        case HttpMethod::PATCH:   return "PATCH";
        case HttpMethod::DELETE:  return "DELETE";
        case HttpMethod::HEAD:    return "HEAD";
        case HttpMethod::OPTIONS: return "OPTIONS";
        default: throw std::invalid_argument("Unknown HttpMethod");
    }
}

#endif //NEXUSCORE_HTTPMETHOD_HPP