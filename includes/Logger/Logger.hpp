#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

/**
 * @brief Thin spdlog wrapper that provides a named, coloured console logger.
 *
 * Each Logger instance maps to a uniquely named spdlog logger registered in
 * the spdlog registry.  If a logger with the same name already exists (e.g.
 * in unit tests), the constructor falls back to the existing one.
 *
 * Log pattern: @c "[HH:MM:SS.mmm] [LEVEL   ] [name] message"
 *
 * @code
 * Logger log{"MyModule"};
 * log.logInfo("Module started");
 * log.logWarning("Reconnecting...");
 * log.logError("Connection failed: timeout");
 * @endcode
 */
class Logger {
public:
    /**
     * @brief Creates a coloured stdout logger with the given name.
     * @param name Identifier shown in the log prefix, e.g. "EventBus".
     */
    explicit Logger(const std::string& name) {
        try {
            _logger = spdlog::stdout_color_mt(name);
            _logger->set_level(spdlog::level::trace);
            _logger->set_pattern("[%H:%M:%S.%e] [%^%-8l%$] [%n] %v");
        } catch (const spdlog::spdlog_ex&) {
            _logger = spdlog::get(name);
        }
    }

    ~Logger() = default;

    /** @brief Logs an informational message. */
    void logInfo(const std::string& message) {
        if (_logger) _logger->info(message);
    }

    /** @brief Logs a warning message. */
    void logWarning(const std::string& message) {
        if (_logger) _logger->warn(message);
    }

    /** @brief Logs an error message. */
    void logError(const std::string& message) {
        if (_logger) _logger->error(message);
    }

    /** @brief Logs a critical (fatal) message. */
    void logCritical(const std::string& message) {
        if (_logger) _logger->critical(message);
    }

private:
    std::shared_ptr<spdlog::logger> _logger;
};

#endif