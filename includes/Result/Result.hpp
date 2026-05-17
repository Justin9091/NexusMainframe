#pragma once
#include <string>

class Result {
public:
    static Result ok        (std::string message = "OK");
    static Result created   (std::string message = "Created");
    static Result badRequest(std::string message = "Bad request");
    static Result notFound  (std::string message = "Not found");
    static Result conflict  (std::string message = "Conflict");
    static Result forbidden (std::string message = "Forbidden");
    static Result failed    (std::string message = "Internal server error");

    bool               isOk()       const;
    int                httpStatus() const;
    const std::string& message()    const;
    std::string        toJson()     const;

protected:
    Result(int httpStatus, std::string message);

private:
    int         _httpStatus;
    std::string _message;
};
