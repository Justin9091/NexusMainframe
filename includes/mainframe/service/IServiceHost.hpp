//
// Created by jusra on 18-1-2026.
//

#ifndef NEXUSCORE_ISERVICEHOST_HPP
#define NEXUSCORE_ISERVICEHOST_HPP

#include <functional>
#include <string>

/**
 * @brief Abstraction for the process host environment (foreground vs. Windows service).
 *
 * run() blocks until a shutdown signal is received (SIGINT in foreground mode,
 * SCM stop command in Windows Service mode), calling @p onStart before the
 * loop and @p onStop on exit.
 *
 * Use ServiceHostFactory::create() to obtain the correct implementation.
 */
class IServiceHost {
public:
    using StartCallback = std::function<void()>; ///< Called once when the host starts.
    using StopCallback  = std::function<void()>; ///< Called once when the host stops.

    virtual ~IServiceHost() = default;

    /**
     * @brief Runs the host's main loop until a stop signal is received.
     * @param onStart  Invoked after the host initialises, before the event loop.
     * @param onStop   Invoked after the stop signal, before returning.
     */
    virtual void run(StartCallback onStart, StopCallback onStop) = 0;
};

#endif //NEXUSCORE_ISERVICEHOST_HPP