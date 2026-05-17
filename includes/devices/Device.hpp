#pragma once
#include <string>
#include <nlohmann/json.hpp>

/**
 * @brief Represents a single device in the system.
 *
 * Devices are persisted to disk by DeviceRegistry and exposed via the HTTP API.
 * The `state` field is a free-form JSON object — its schema is defined by the
 * device type (e.g. a light may have `{"brightness": 80, "on": true}`).
 *
 * @see DeviceRegistry, DeviceService
 */
struct Device {
    std::string    id;     ///< Unique identifier (UUID, assigned on creation).
    std::string    name;   ///< Human-readable display name.
    std::string    type;   ///< Device type string, e.g. `"light"`, `"sensor"`.
    nlohmann::json state  = nlohmann::json::object(); ///< Arbitrary device state.
    bool           online = false; ///< Whether the device is currently reachable.

    /**
     * @brief Serializes the device to a JSON object.
     * @return JSON representation with all fields.
     */
    nlohmann::json toJson() const;

    /**
     * @brief Deserializes a device from a JSON object.
     * @param j JSON object containing at least `id`, `name`, and `type`.
     * @return Populated Device instance.
     */
    static Device  fromJson(const nlohmann::json& j);
};
