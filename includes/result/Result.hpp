#pragma once
#include <string>

/**
 * @brief Represents the outcome of a service operation, mapped to an HTTP status.
 *
 * Used by service methods to communicate success or failure without throwing.
 * The caller can forward the result directly to an HTTP response via
 * httpStatus() and toJson().
 *
 * @code
 * Result r = deviceService.add(device);
 * if (!r.isOk()) {
 *     res.status = r.httpStatus();
 *     res.set_content(r.toJson(), "application/json");
 *     return;
 * }
 * @endcode
 */
class Result {
public:
    static Result ok        (std::string message = "OK");                     ///< HTTP 200.
    static Result created   (std::string message = "Created");                ///< HTTP 201.
    static Result badRequest(std::string message = "Bad request");            ///< HTTP 400.
    static Result notFound  (std::string message = "Not found");              ///< HTTP 404.
    static Result conflict  (std::string message = "Conflict");               ///< HTTP 409.
    static Result forbidden (std::string message = "Forbidden");              ///< HTTP 403.
    static Result failed    (std::string message = "Internal server error");  ///< HTTP 500.

    /** @return @c true if the HTTP status code is in the 2xx range. */
    bool               isOk()       const;

    /** @return The HTTP status code (e.g. 200, 404). */
    int                httpStatus() const;

    /** @return The human-readable status message. */
    const std::string& message()    const;

    /** @return JSON string: @c {"success":true,"message":"OK"} */
    std::string        toJson()     const;

protected:
    Result(int httpStatus, std::string message);

private:
    int         _httpStatus;
    std::string _message;
};
