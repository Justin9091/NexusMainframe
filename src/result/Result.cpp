#include "result/Result.hpp"
#include <nlohmann/json.hpp>

Result::Result(int httpStatus, std::string message)
    : _httpStatus(httpStatus), _message(std::move(message)) {}

Result Result::ok        (std::string message) { return {200, std::move(message)}; }
Result Result::created   (std::string message) { return {201, std::move(message)}; }
Result Result::badRequest(std::string message) { return {400, std::move(message)}; }
Result Result::notFound  (std::string message) { return {404, std::move(message)}; }
Result Result::conflict  (std::string message) { return {409, std::move(message)}; }
Result Result::forbidden (std::string message) { return {403, std::move(message)}; }
Result Result::failed    (std::string message) { return {500, std::move(message)}; }

bool               Result::isOk()       const { return _httpStatus >= 200 && _httpStatus < 300; }
int                Result::httpStatus() const { return _httpStatus; }
const std::string& Result::message()    const { return _message; }

std::string Result::toJson() const {
    return nlohmann::json{{"success", isOk()}, {"output", _message}}.dump();
}
