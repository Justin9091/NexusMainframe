#pragma once
#include "IService.hpp"

/**
 * @brief IService wrapper around LogBroadcaster.
 *
 * Starts and stops the LogBroadcaster WebSocket server as part of the
 * NexusMainFrame service lifecycle.
 */
class LogService : public IService {
public:
    /**
     * @param port TCP port for the log WebSocket server (default 8084).
     */
    explicit LogService(int port = 8084);

    std::string_view getName() const override { return "LogService"; }

    /** @brief Starts the LogBroadcaster WebSocket server. */
    void start() override;

    /** @brief Stops the LogBroadcaster WebSocket server. */
    void stop() override;

private:
    int _port;
};
