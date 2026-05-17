//
// Created by jusra on 4-1-2026.
//

#ifndef NEXUSCORE_CONFIGPROVIDER_HPP
#define NEXUSCORE_CONFIGPROVIDER_HPP
#include <fstream>
#include <unordered_map>
#include <memory>
#include <optional>
#include <typeindex>

#include "config/Adapter/IConfigAdapter.hpp"
#include "config/Source/IConfigSource.hpp"
#include "config/Source/JsonConfigSource.hpp"
/**
 * @brief Type-safe configuration store backed by a file source and typed adapters.
 *
 * ConfigProvider owns a ConfigMap (loaded from an IConfigSource) and a registry
 * of IConfigAdapter<T> adapters.  Typed structs are stored and retrieved via
 * setStruct<T>() / getStruct<T>() using the appropriate adapter.
 *
 * @code
 * auto provider = ConfigProviderFactory::create("config.json");
 * provider.registerAdapter<ModuleEntry>(
 *     std::make_shared<LoadedModuleAdapter>());
 * provider.load("config.json");
 *
 * auto entry = provider.getStruct<ModuleEntry>("modules");
 * @endcode
 */
class ConfigProvider {
public:
    /**
     * @param source  Config source used for save(); the actual data is loaded
     *                separately via load().
     */
    explicit ConfigProvider(std::unique_ptr<IConfigSource> source)
        : source_(std::move(source)) {}

    /**
     * @brief Registers a typed adapter for serialisation/deserialisation.
     * @tparam T      Struct type to handle.
     * @param adapter  Adapter implementation.
     */
    template<typename T>
    void registerAdapter(std::shared_ptr<IConfigAdapter<T>> adapter) {
        adapters_[typeid(T)] = adapter;
    }

    /**
     * @brief Serialises a struct and stores it under @p key.
     * @tparam T    Struct type; an adapter must be registered for it.
     * @param key   Config map key.
     * @param value Value to store.
     * @throws std::runtime_error if no adapter is registered for T.
     */
    template<typename T>
    void setStruct(const std::string &key, const T &value) {
        auto it = adapters_.find(typeid(T));
        if (it == adapters_.end()) {
            throw std::runtime_error("No adapter registered for this type");
        }

        auto adapter = std::static_pointer_cast<IConfigAdapter<T>>(it->second);
        config_[key] = adapter->serialize(value);
    }

    /**
     * @brief Deserialises a struct stored under @p key.
     * @tparam T   Struct type; an adapter must be registered for it.
     * @param key  Config map key.
     * @return The deserialised struct, or std::nullopt if the key is absent.
     * @throws std::runtime_error if no adapter is registered for T or the
     *         stored value is not a JSON object.
     */
    template<typename T>
    std::optional<T> getStruct(const std::string &key) const {
        auto it = config_.find(key);
        if (it == config_.end()) return std::nullopt;

        auto adapterIt = adapters_.find(typeid(T));
        if (adapterIt == adapters_.end()) {
            throw std::runtime_error("No adapter registered for this type");
        }

        auto jsonVal = std::get_if<nlohmann::json>(&it->second);
        if (!jsonVal) {
            throw std::runtime_error("Config value is not JSON");
        }

        auto adapter = std::static_pointer_cast<IConfigAdapter<T>>(adapterIt->second);
        return adapter->deserialize(*jsonVal);
    }

    /**
     * @brief Loads all key/value pairs from a JSON file into the internal map.
     * @param filepath  Absolute path to the JSON file.
     * @throws std::runtime_error if the file cannot be opened or is not valid JSON.
     */
    void load(const std::string& filepath) {
        std::ifstream in(filepath);
        if (!in.is_open()) {
            throw std::runtime_error("Failed to open file for loading config: " + filepath);
        }

        nlohmann::json j;
        in >> j;

        config_.clear();
        for (auto it = j.begin(); it != j.end(); ++it) {
            config_[it.key()] = it.value();
        }
    }

    /**
     * @brief Serialises the internal map to a pretty-printed JSON file.
     * @param filepath  Absolute path to the output file.
     * @throws std::runtime_error if the file cannot be opened.
     */
    void save(const std::string& filepath) {
        if (!source_) {
            throw std::runtime_error("No config source available to save");
        }

        nlohmann::json j;

        for (const auto& [key, value] : config_) {
            std::visit([&](auto&& v) {
                j[key] = v;
            }, value);
        }

        std::ofstream out(filepath);
        if (!out.is_open()) {
            throw std::runtime_error("Failed to open file for saving config: " + filepath);
        }

        out << j.dump(4);
    }
};

#endif //NEXUSCORE_CONFIGPROVIDER_HPP
