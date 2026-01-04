#ifndef NEXUSMAINFRAME_HPP
#define NEXUSMAINFRAME_HPP

#include <memory>
#include <atomic>

#include "Event/EventBus.hpp"
#include "Modules/ModuleLoader.hpp"
#include "Scheduler/Scheduler.hpp"

class NexusMainFrame {
private:
    std::unique_ptr<ModuleLoader> _moduleLoader;
    std::atomic<bool> _running{true};
    Scheduler _scheduler;
    EventBus _eventBus;

public:
    void run();
    void start();
    void stop();
};

#endif
