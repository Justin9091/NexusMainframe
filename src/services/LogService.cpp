#include "services/LogService.hpp"
#include "logging/LogBroadcaster.hpp"

LogService::LogService(int port) : _port(port) {}

void LogService::start() {
    LogBroadcaster::getInstance().start(_port);
}

void LogService::stop() {
    LogBroadcaster::getInstance().stop();
}
