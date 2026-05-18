// IConfigAdapter.hpp
#ifndef NEXUSCORE_ICONFIGADAPTER_HPP
#define NEXUSCORE_ICONFIGADAPTER_HPP
#include "nlohmann/json.hpp"
#include "config/Source/IConfigSource.hpp"
#include "nlohmann/json.hpp"

/**
 * @brief Bidirectional serialisation adapter between a C++ struct and JSON.
 *
 * Register a concrete adapter with ConfigProvider::registerAdapter<T>() to
 * enable setStruct<T>() / getStruct<T>() for that type.
 *
 * @tparam T  The C++ struct type this adapter handles.
 *
 * @code
 * struct MyAdapter : IConfigAdapter<MyConfig> {
 *     nlohmann::json serialize(const MyConfig& v) const override {
 *         return {{"key", v.key}};
 *     }
 *     MyConfig deserialize(const nlohmann::json& j) const override {
 *         return {j.at("key").get<std::string>()};
 *     }
 * };
 * provider.registerAdapter<MyConfig>(std::make_shared<MyAdapter>());
 * @endcode
 */
template<typename T>
struct IConfigAdapter {
    virtual ~IConfigAdapter() = default;

    /**
     * @brief Converts a struct to a JSON object for storage.
     * @param value  Value to serialise.
     * @return JSON representation.
     */
    virtual nlohmann::json serialize(const T& value) const = 0;

    /**
     * @brief Reconstructs a struct from a JSON object.
     * @param value  JSON to deserialise.
     * @return Populated struct.
     */
    virtual T deserialize(const nlohmann::json& value) const = 0;
};

#endif // NEXUSCORE_ICONFIGADAPTER_HPP
