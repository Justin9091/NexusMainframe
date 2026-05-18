#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

/**
 * @brief Represents a logical room that groups one or more devices.
 *
 * Rooms are persisted in JSON by RoomRegistry and exposed via the REST API
 * through RoomController.
 */
struct Room {
    std::string              id;        ///< UUID assigned on creation.
    std::string              name;      ///< Human-readable room name.
    std::vector<std::string> deviceIds; ///< IDs of devices assigned to this room.

    /** @brief Serialises the room to a JSON object. */
    nlohmann::json toJson() const;

    /**
     * @brief Deserialises a room from a JSON object.
     * @param j  JSON object with @c id, @c name, and @c deviceIds fields.
     */
    static Room fromJson(const nlohmann::json& j);
};
