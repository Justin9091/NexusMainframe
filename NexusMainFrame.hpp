#pragma once
#include <memory>
#include <vector>
#include <atomic>
#include <thread>
#include "includes/services/IService.hpp"
#include "Scheduler/Scheduler.hpp"
#include "config/NexusConfig.hpp"

class NexusMainFrame {
public:
    void addService(std::unique_ptr<IService> service);
    void run();
    void stop();

private:
    std::vector<std::unique_ptr<IService>> _services;
    Scheduler          _scheduler;
    std::atomic<bool>  _running{false};
    NexusConfig        _config;

    void initPaths();
    void registerBuiltinServices();
    void startAll();
    void stopAll();
};
