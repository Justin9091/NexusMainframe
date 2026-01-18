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
// ConfigProvider.hpp
class ConfigProvider {
private:
    ConfigMap config_;
    std::unique_ptr<IConfigSource> source_;
    std::unordered_map<std::type_index, std::shared_ptr<void>> adapters_;

public:
    explicit ConfigProvider(std::unique_ptr<IConfigSource> source)
        : source_(std::move(source)) {
    }

    // Adapter registreren
    template<typename T>
    void registerAdapter(std::shared_ptr<IConfigAdapter<T>> adapter) {
        adapters_[typeid(T)] = adapter;
    }

    // Struct opslaan via adapter
    template<typename T>
    void setStruct(const std::string &key, const T &value) {
        auto it = adapters_.find(typeid(T));
        if (it == adapters_.end()) {
            throw std::runtime_error("No adapter registered for this type");
        }

        auto adapter = std::static_pointer_cast<IConfigAdapter<T>>(it->second);
        config_[key] = adapter->serialize(value);
    }

    // Struct ophalen via adapter
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

    // 👇 VOEG DEZE METHODE TOE
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
