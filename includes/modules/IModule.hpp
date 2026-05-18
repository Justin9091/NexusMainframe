//
// Created by jusra on 29-12-2025.
//

#ifndef NEXUSMAINFRAME_IMODULE_HPP
#define NEXUSMAINFRAME_IMODULE_HPP

#include <string>

#include "Event/EventBus.hpp"
#include "Logger/Logger.hpp"

/**
 * @brief Base interface for all NexusMainFrame modules.
 *
 * Modules are dynamically loaded shared libraries (.dll on Windows, .so on Linux)
 * that extend the framework at runtime. Every module must inherit from this interface
 * and export a C factory function so the loader can instantiate it:
 *
 * @code{.cpp}
 * // Windows
 * extern "C" __declspec(dllexport) IModule* createModule() {
 *     return new MyModule();
 * }
 *
 * // Linux
 * extern "C" __attribute__((visibility("default"))) IModule* createModule() {
 *     return new MyModule();
 * }
 * @endcode
 *
 * **Lifecycle**
 *
 * The ModuleLoader calls methods in this order:
 * 1. `createModule()` — factory, allocates the instance
 * 2. `initialize()` — subscribe to events, start threads, open connections
 * 3. *(module runs)*
 * 4. `shutdown()` — release all resources before the library is unloaded
 *
 * **Thread safety**
 *
 * `initialize()` and `shutdown()` are called from the main thread.
 * EventBus callbacks can fire from any thread — protect shared state with a mutex.
 *
 * @see EventBus, MODULE_TEMPLATE.md
 */
class IModule {
public:
    virtual ~IModule() = default;

    /**
     * @brief Returns the unique name of this module.
     *
     * Used as the identifier in HTTP API calls (`/api/modules/{name}/enable`),
     * log output, and the module registry. Must be stable across runs.
     *
     * @return Non-empty module name, e.g. `"TemperatureSensor"`.
     */
    virtual std::string getName() const = 0;

    /**
     * @brief Called once after the module is loaded into memory.
     *
     * Subscribe to EventBus topics and set up resources here.
     * The @p eventBus reference stays valid for the entire lifetime of the module.
     *
     * @code{.cpp}
     * void MyModule::initialize(EventBus& eventBus) {
     *     _subId = eventBus.subscribe("sensor.reading", [this](const Event& e) {
     *         // handle event
     *     });
     * }
     * @endcode
     *
     * @param eventBus  The application-wide event bus (singleton).
     * @warning Do not store a raw pointer to @p eventBus — use the reference
     *          directly or call `EventBus::getInstance()` later.
     */
    virtual void initialize(EventBus& eventBus) = 0;

    /**
     * @brief Called once before the module is unloaded.
     *
     * Release all resources acquired in `initialize()`: stop threads, close
     * connections, and unsubscribe from the EventBus using the IDs returned
     * by `subscribe()`.
     *
     * @code{.cpp}
     * void MyModule::shutdown() {
     *     EventBus::getInstance().unsubscribe("sensor.reading", _subId);
     * }
     * @endcode
     *
     * @warning The EventBus may already be shutting down — do not publish
     *          events from this method.
     */
    virtual void shutdown() = 0;

    /**
     * @brief Returns a logger scoped to this module.
     *
     * Backed by spdlog. The logger name is set to `getName()` automatically.
     * Call once and store the result rather than calling this on every log line.
     *
     * @code{.cpp}
     * auto log = getLogger();
     * log.logInfo("Module started");
     * @endcode
     *
     * @return Logger instance with the module name as the channel name.
     */
    Logger getLogger() const {
        Logger logger {getName()};
        return logger;
    }
};

#endif //NEXUSMAINFRAME_IMODULE_HPP