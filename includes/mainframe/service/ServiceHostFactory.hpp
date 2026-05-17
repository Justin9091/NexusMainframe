//
// Created by jusra on 18-1-2026.
//

#ifndef NEXUSCORE_SERVICEHOSTFACTORY_HPP
#define NEXUSCORE_SERVICEHOSTFACTORY_HPP

#include <memory>
#include "IServiceHost.hpp"

/**
 * @brief Factory that creates the platform-appropriate IServiceHost.
 *
 * Returns a WindowsServiceHost when compiled with @c _WIN32 and a
 * ForegroundServiceHost otherwise.
 */
class ServiceHostFactory {
public:
    /**
     * @brief Creates the platform-specific IServiceHost.
     * @return Owning pointer to the host implementation.
     */
    static std::unique_ptr<IServiceHost> create();
};

#endif //NEXUSCORE_SERVICEHOSTFACTORY_HPP