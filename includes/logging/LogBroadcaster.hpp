#pragma once
#include <deque>
#include <mutex>
#include <string>
#include "server/WsServer.hpp"

/**
 * @brief A single log entry stored in the LogBroadcaster ring buffer.
 */
struct LogEntry {
    std::string level;     ///< Severity: @c "debug", @c "info", @c "warn", or @c "error".
    std::string source;    ///< Component that produced the log (e.g. a module name).
    std::string message;   ///< Log message text.
    std::string timestamp; ///< ISO 8601 timestamp (e.g. @c "2026-01-12T14:23:01Z").
};

/**
 * @brief Singleton log aggregator that streams entries to a WebSocket server.
 *
 * Modules write via WebLogger, which calls LogBroadcaster::log().  The
 * broadcaster maintains a ring buffer of the last BUFFER_MAX entries and
 * serves them to newly connected WebSocket clients (backfill of BACKFILL_COUNT
 * entries), then streams subsequent entries live.
 *
 * The WebSocket server listens on port 8084 by default.  Each frame is a JSON
 * object:
 * @code
 * {"level":"info","source":"my-module","message":"Started","timestamp":"..."}
 * @endcode
 *
 * @note LogService is the IService wrapper; use it to integrate with NexusMainFrame.
 */
class LogBroadcaster {
public:
    /// Returns the process-wide singleton instance.
    static LogBroadcaster& getInstance();

    LogBroadcaster(const LogBroadcaster&)            = delete;
    LogBroadcaster& operator=(const LogBroadcaster&) = delete;

    /**
     * @brief Starts the WebSocket server.
     * @param port TCP port to listen on (default 8084).
     */
    void start(int port = 8084);

    /** @brief Stops the WebSocket server. */
    void stop();

    /**
     * @brief Appends an entry to the ring buffer and broadcasts it to all clients.
     * @param level    Severity string (@c "debug", @c "info", @c "warn", @c "error").
     * @param source   Originating component name.
     * @param message  Log message text.
     */
    void log(const std::string& level,
             const std::string& source,
             const std::string& message);

private:
    LogBroadcaster() = default;

    void onClientConnect(int clientId);
    std::string entryToJson(const LogEntry& entry) const;
    static std::string nowIso();

    static constexpr size_t BUFFER_MAX     = 500; ///< Maximum ring buffer size.
    static constexpr size_t BACKFILL_COUNT = 100; ///< Entries sent to a newly connected client.

    std::mutex           _mu;
    std::deque<LogEntry> _buffer;
    WsServer             _ws;
};
