#pragma once
#include <deque>
#include <mutex>
#include <string>
#include "server/WsServer.hpp"

struct LogEntry {
    std::string level;      // "debug" | "info" | "warn" | "error"
    std::string source;
    std::string message;
    std::string timestamp;  // ISO 8601
};

// Singleton — owns the log ringbuffer and the WebSocket server on port 8084.
// Modules use WebLogger to write to it; the frontend receives live JSON frames.
class LogBroadcaster {
public:
    static LogBroadcaster& getInstance();

    LogBroadcaster(const LogBroadcaster&)            = delete;
    LogBroadcaster& operator=(const LogBroadcaster&) = delete;

    void start(int port = 8084);
    void stop();

    void log(const std::string& level,
             const std::string& source,
             const std::string& message);

private:
    LogBroadcaster() = default;

    void onClientConnect(int clientId);
    std::string entryToJson(const LogEntry& entry) const;
    static std::string nowIso();

    static constexpr size_t BUFFER_MAX     = 500;
    static constexpr size_t BACKFILL_COUNT = 100;

    std::mutex           _mu;
    std::deque<LogEntry> _buffer;
    WsServer             _ws;
};
