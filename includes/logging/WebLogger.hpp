#pragma once
#include <string>

/**
 * @brief Drop-in logger that streams log entries to the frontend dashboard.
 *
 * Forwards calls to LogBroadcaster::log(), which broadcasts them over
 * WebSocket (port 8084) to any connected frontend client.  Create one
 * instance per logical component; the @p source string identifies it in logs.
 *
 * @code
 * WebLogger log{"my-module"};
 * log.info("Module started");
 * log.warn("Sensor value out of range");
 * log.error("Connection refused");
 * @endcode
 *
 * For one-off logging without an instance:
 * @code
 * WebLogger::emit("info", "startup", "All systems nominal");
 * @endcode
 */
class WebLogger {
public:
    /**
     * @param source  Component name shown in the dashboard (e.g. "my-module").
     */
    explicit WebLogger(std::string source);

    /** @brief Logs a debug-level message. */
    void debug(const std::string& message) const;

    /** @brief Logs an info-level message. */
    void info (const std::string& message) const;

    /** @brief Logs a warning-level message. */
    void warn (const std::string& message) const;

    /** @brief Logs an error-level message. */
    void error(const std::string& message) const;

    /**
     * @brief Static convenience form — does not require a WebLogger instance.
     * @param level    Severity string (@c "debug", @c "info", @c "warn", @c "error").
     * @param source   Originating component name.
     * @param message  Log message text.
     */
    static void emit(const std::string& level,
                     const std::string& source,
                     const std::string& message);

private:
    std::string _source;
};
