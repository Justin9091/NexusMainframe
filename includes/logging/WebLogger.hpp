#pragma once
#include <string>

// Drop-in logger that streams to the frontend dashboard over WebSocket.
// Create one per module: WebLogger log{"my-module"};
// Then: log.info("connected"); log.error("timeout");
class WebLogger {
public:
    explicit WebLogger(std::string source);

    void debug(const std::string& message) const;
    void info (const std::string& message) const;
    void warn (const std::string& message) const;
    void error(const std::string& message) const;

    // Static form when you don't want a WebLogger instance.
    static void emit(const std::string& level,
                     const std::string& source,
                     const std::string& message);

private:
    std::string _source;
};
