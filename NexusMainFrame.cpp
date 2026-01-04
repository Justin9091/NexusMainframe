#include "NexusMainFrame.hpp"

#include <iostream>
#include <thread>

#include "Event/EventBus.hpp"
#include "Modules/ModuleLoader.hpp"

void NexusMainFrame::start() {
    _moduleLoader = std::make_unique<ModuleLoader>();
    _moduleLoader->loadModulesFromDirectory("./modules");

    for (auto &m: _moduleLoader->getLoadedModules()) {
        m->initialize(_eventBus);
    }
}

void NexusMainFrame::stop() {
    _eventBus.shutdown();
    for (auto &m: _moduleLoader->getLoadedModules())
        m->shutdown();
}

void NexusMainFrame::run() {
    start();

    while (_running.load()) {
        _eventBus.dispatchPending();
        _scheduler.tick();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    stop();
}
