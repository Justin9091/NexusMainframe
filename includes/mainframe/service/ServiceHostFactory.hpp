//
// Created by jusra on 18-1-2026.
//

#ifndef NEXUSCORE_SERVICEHOSTFACTORY_HPP
#define NEXUSCORE_SERVICEHOSTFACTORY_HPP

#include <memory>
#include "IServiceHost.hpp"

class ServiceHostFactory {
public:
    static std::unique_ptr<IServiceHost> create();
};

#endif //NEXUSCORE_SERVICEHOSTFACTORY_HPP