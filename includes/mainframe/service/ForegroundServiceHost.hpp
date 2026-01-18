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
class ForegroundServiceHost : public IServiceHost {
private:
    static std::atomic<bool> running;

public:
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