#pragma once
#include <memory>
#include <vector>
#include <atomic>
#include <thread>
#include "includes/services/IService.hpp"
#include "Scheduler/Scheduler.hpp"
#include "config/NexusConfig.hpp"

/**
 * @brief Top-level application host for the Nexus framework.
 *
 * NexusMainFrame owns the service registry and drives the main run-loop.
 * Typical usage:
 * @code
 * NexusMainFrame app;
 * app.run();  // blocks until stop() is called or SIGINT received
 * @endcode
 *
 * Services are registered automatically via registerBuiltinServices().
 * Additional services can be injected before run() with addService().
 */
class NexusMainFrame {
public:
    /**
     * @brief Registers an additional service to be started with the framework.
     * @param service Owning pointer to the service; must be non-null.
     * @note Call before run().
     */
    void addService(std::unique_ptr<IService> service);

    /**
     * @brief Starts all services and blocks until stop() is called.
     *
     * Initialises file-system paths, loads NexusConfig, registers built-in
     * services, then enters a spin-loop calling IService::update() on each
     * service every iteration.
     */
    void run();

    /**
     * @brief Signals the run-loop to exit and stops all services.
     * Thread-safe; safe to call from a signal handler.
     */
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
