//
// Created by jusra on 18-1-2026.
//

#ifndef NEXUSCORE_FOREGROUNDSERVICEHOST_HPP
#define NEXUSCORE_FOREGROUNDSERVICEHOST_HPP

#include "IServiceHost.hpp"
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>
/**
 * @brief IServiceHost implementation for interactive (terminal) execution.
 *
 * Installs a SIGINT handler so Ctrl+C triggers a clean shutdown.
 * Suitable for development and Linux/macOS deployments that do not run as
 * a system service.
 */
class ForegroundServiceHost : public IServiceHost {
private:
    static std::atomic<bool> running;

public:
    /**
     * @brief Starts the application and blocks until SIGINT is received.
     * @param onStart  Called once before entering the wait loop.
     * @param onStop   Called once after SIGINT, before returning.
     */
    void run(StartCallback onStart, StopCallback onStop) override {
        running = true;

        std::signal(SIGINT, [](int) {
            running = false;
        });

        onStart();

        while (running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        onStop();
    }
};

std::atomic<bool> ForegroundServiceHost::running{false};

#endif //NEXUSCORE_FOREGROUNDSERVICEHOST_HPP