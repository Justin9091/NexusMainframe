// HttpMethod.hpp

#ifndef NEXUSCORE_HTTPMETHOD_HPP
#define NEXUSCORE_HTTPMETHOD_HPP

#include <string>
#include <stdexcept>

// Windows defines DELETE as a macro in <windows.h>; undefine before our enum
#ifdef DELETE
#undef DELETE
#endif

/**
 * @brief Enumeration of supported HTTP request methods.
 *
 * @note The @c DELETE enumerator conflicts with the Windows SDK macro of the
 *       same name.  The macro is undefined at the top of this header.
 */
enum class HttpMethod {
    GET,     ///< HTTP GET — retrieve a resource.
    POST,    ///< HTTP POST — create a resource.
    PATCH,   ///< HTTP PATCH — partially update a resource.
    PUT,     ///< HTTP PUT — replace a resource.
    DELETE,  ///< HTTP DELETE — remove a resource.
    HEAD,    ///< HTTP HEAD — retrieve headers only.
    OPTIONS  ///< HTTP OPTIONS — query supported methods.
};

/**
 * @brief Converts an HttpMethod enumerator to its uppercase string representation.
 * @param method  HTTP method value.
 * @return String such as @c "GET", @c "POST", etc.
 * @throws std::invalid_argument if @p method is an unrecognised value.
 */
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