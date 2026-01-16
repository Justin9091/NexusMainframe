//
// Created by jusra on 4-1-2026.
//

#ifndef NEXUSCORE_CONFIGPROVIDERFACTORY_HPP
#define NEXUSCORE_CONFIGPROVIDERFACTORY_HPP

#include "Provider/ConfigProvider.hpp"

class ConfigProviderFactory {
public:
    static ConfigProvider create(const std::string& filepath);
};


#endif //NEXUSCORE_CONFIGPROVIDERFACTORY_HPP