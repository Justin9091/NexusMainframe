#pragma once
#include <string_view>

/**
 * @brief Base interface for all Nexus services.
 *
 * Services are long-running components managed by NexusMainFrame.  The
 * framework calls start() on each service during boot, update() on every
 * main-loop iteration, and stop() during shutdown — in reverse registration
 * order.
 *
 * Concrete services should override getName(), start(), and stop() at minimum.
 * Override update() only if the service requires periodic polling.
 */
class IService {
public:
    virtual ~IService() = default;

    /** @brief Returns a human-readable name used in logs (e.g. "HttpService"). */
    virtual std::string_view getName() const = 0;

    /** @brief Called once during framework start-up to initialise the service. */
    virtual void start() = 0;

    /** @brief Called on every main-loop iteration; default is a no-op. */
    virtual void update() {}

    /** @brief Called during framework shutdown to cleanly tear down the service. */
    virtual void stop() = 0;
};
