//
// Created by jusra on 4-1-2026.
//

#ifndef NEXUSCORE_CONFIGPROVIDERFACTORY_HPP
#define NEXUSCORE_CONFIGPROVIDERFACTORY_HPP

#include "Provider/ConfigProvider.hpp"

/**
 * @brief Factory that creates a ConfigProvider pre-loaded from a JSON file.
 */
class ConfigProviderFactory {
public:
    /**
     * @brief Creates a ConfigProvider backed by JsonConfigSource and loads @p filepath.
     * @param filepath  Absolute path to the JSON configuration file.
     * @return Populated ConfigProvider.
     * @throws std::runtime_error if the file cannot be read or parsed.
     */
    static ConfigProvider create(const std::string& filepath);
};


#endif //NEXUSCORE_CONFIGPROVIDERFACTORY_HPP