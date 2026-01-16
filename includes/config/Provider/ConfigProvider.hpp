//
// Created by jusra on 4-1-2026.
//

#ifndef NEXUSCORE_CONFIGPROVIDER_HPP
#define NEXUSCORE_CONFIGPROVIDER_HPP

#include <memory>
#include <optional>
#include <string>

#include "config/Source/IConfigSource.hpp"

class ConfigProvider {
private:
    ConfigMap config_;
    std::unique_ptr<IConfigSource> source_;

public:
    explicit ConfigProvider(std::unique_ptr<IConfigSource> source);

    void load(const std::string &filepath);
    void save(const std::string &filepath);

    bool has(const std::string &key) const;

    template<typename T>
    std::optional<T> get(const std::string &key) const {
        auto it = config_.find(key);
        if (it == config_.end()) {
            return std::nullopt;
        }

        if (auto *val = std::get_if<T>(&it->second)) {
            return *val;
        }

        return std::nullopt;
    }

    template<typename T>
    T getOrDefault(const std::string &key, T defaultValue) const {
        return get<T>(key).value_or(defaultValue);
    }

    template<typename T>
    void set(const std::string &key, const T &value) {
        config_[key] = value;
    }

    void remove(const std::string &key);
    void clear();
};

#endif //NEXUSCORE_CONFIGPROVIDER_HPP