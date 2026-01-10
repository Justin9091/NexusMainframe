#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

class Logger {
private:
    std::shared_ptr<spdlog::logger> _logger;

public:
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

    void logInfo(const std::string& message) {
        if (_logger) _logger->info(message);
    }

    void logWarning(const std::string& message) {
        if (_logger) _logger->warn(message);
    }

    void logError(const std::string& message) {
        if (_logger) _logger->error(message);
    }

    void logCritical(const std::string& message) {
        if (_logger) _logger->critical(message);
    }
};

#endif