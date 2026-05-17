#include "logging/LogBroadcaster.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

LogBroadcaster& LogBroadcaster::getInstance() {
    static LogBroadcaster instance;
    return instance;
}

void LogBroadcaster::start(int port) {
    _ws.onConnect([this](int id) { onClientConnect(id); });
    _ws.start(port);
    std::cout << "[LogBroadcaster] Log stream on port " << port << "\n";
}

void LogBroadcaster::stop() {
    _ws.stop();
}

// ── Public API ────────────────────────────────────────────────────────────────

void LogBroadcaster::log(const std::string& level,
                         const std::string& source,
                         const std::string& message) {
    LogEntry entry{level, source, message, nowIso()};
    std::string frame = entryToJson(entry);

    {
        std::lock_guard lock(_mu);
        if (_buffer.size() >= BUFFER_MAX)
            _buffer.pop_front();
        _buffer.push_back(entry);
    }

    _ws.broadcast(frame);
}

// ── Backfill on connect ───────────────────────────────────────────────────────

void LogBroadcaster::onClientConnect(int clientId) {
    std::vector<LogEntry> snapshot;
    {
        std::lock_guard lock(_mu);
        size_t start = _buffer.size() > BACKFILL_COUNT
                           ? _buffer.size() - BACKFILL_COUNT : 0;
        snapshot.assign(_buffer.begin() + (long long)start, _buffer.end());
    }
    for (const auto& e : snapshot)
        _ws.send(clientId, entryToJson(e));
}

// ── Helpers ───────────────────────────────────────────────────────────────────

std::string LogBroadcaster::entryToJson(const LogEntry& entry) const {
    // Hand-built to avoid pulling nlohmann into every translation unit that
    // includes LogBroadcaster — but nlohmann is available via PCH so either works.
    nlohmann::json j = {
        {"level",     entry.level},
        {"source",    entry.source},
        {"message",   entry.message},
        {"timestamp", entry.timestamp}
    };
    return j.dump();
}

std::string LogBroadcaster::nowIso() {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto tt  = system_clock::to_time_t(now);
    auto ms  = duration_cast<milliseconds>(now.time_since_epoch()).count() % 1000;

    std::tm tm{};
#ifdef _WIN32
    gmtime_s(&tm, &tt);
#else
    gmtime_r(&tt, &tm);
#endif
    std::ostringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S")
       << '.' << std::setfill('0') << std::setw(3) << ms << 'Z';
    return ss.str();
}
