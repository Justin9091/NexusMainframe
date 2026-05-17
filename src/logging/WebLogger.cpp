#include "logging/WebLogger.hpp"
#include "logging/LogBroadcaster.hpp"

WebLogger::WebLogger(std::string source) : _source(std::move(source)) {}

void WebLogger::debug(const std::string& msg) const { emit("debug", _source, msg); }
void WebLogger::info (const std::string& msg) const { emit("info",  _source, msg); }
void WebLogger::warn (const std::string& msg) const { emit("warn",  _source, msg); }
void WebLogger::error(const std::string& msg) const { emit("error", _source, msg); }

void WebLogger::emit(const std::string& level,
                     const std::string& source,
                     const std::string& message) {
    LogBroadcaster::getInstance().log(level, source, message);
}
