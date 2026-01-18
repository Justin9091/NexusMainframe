// IConfigAdapter.hpp
#ifndef NEXUSCORE_ICONFIGADAPTER_HPP
#define NEXUSCORE_ICONFIGADAPTER_HPP
#include "nlohmann/json.hpp"
#include "config/Source/IConfigSource.hpp"
#include "nlohmann/json.hpp"

template<typename T>
struct IConfigAdapter {
    virtual ~IConfigAdapter() = default;

    // Struct → JSON (wordt in ConfigValue opgeslagen)
    virtual nlohmann::json serialize(const T& value) const = 0;

    // JSON → Struct
    virtual T deserialize(const nlohmann::json& value) const = 0;
};

#endif // NEXUSCORE_ICONFIGADAPTER_HPP
