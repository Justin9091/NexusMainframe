#pragma once
#include <filesystem>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "Rooms/Room.hpp"

/**
 * @brief Thread-safe in-memory registry for Room objects with JSON persistence.
 *
 * Rooms are keyed by their UUID.  The registry is persisted to a JSON file on
 * every mutation (add, remove, addDevice, removeDevice).
 */
class RoomRegistry {
public:
    /**
     * @param persistPath  Absolute path to the JSON file used for persistence.
     */
    explicit RoomRegistry(std::filesystem::path persistPath);

    /**
     * @brief Loads rooms from the JSON file into memory.
     * If the file does not exist, the registry starts empty.
     */
    void load();

    /** @brief Writes all rooms to the JSON persistence file. */
    void save() const;

    /**
     * @brief Adds a new room.
     * @param room  Room to add; its @c id must be unique.
     * @return @c true if added; @c false if a room with the same ID already exists.
     */
    bool add(Room room);

    /**
     * @brief Retrieves a room by ID.
     * @param id Room UUID.
     * @return The Room, or std::nullopt if not found.
     */
    std::optional<Room> get(const std::string& id) const;

    /** @brief Returns a snapshot of all rooms. */
    std::vector<Room> list() const;

    /**
     * @brief Checks whether a room with the given ID exists.
     * @param id Room UUID.
     * @return @c true if found.
     */
    bool exists(const std::string& id) const;

    /**
     * @brief Removes a room by ID.
     * @param id Room UUID.
     * @return @c true if removed; @c false if not found.
     */
    bool remove(const std::string& id);

    /**
     * @brief Adds a device ID to a room's device list.
     * @param roomId    Room UUID.
     * @param deviceId  Device ID to add.
     * @return @c true on success; @c false if the room does not exist.
     */
    bool addDevice(const std::string& roomId, const std::string& deviceId);

    /**
     * @brief Removes a device ID from a room's device list.
     * @param roomId    Room UUID.
     * @param deviceId  Device ID to remove.
     * @return @c true on success; @c false if the room or device is not found.
     */
    bool removeDevice(const std::string& roomId, const std::string& deviceId);

private:
    std::filesystem::path                 _path;
    std::unordered_map<std::string, Room> _rooms;
    mutable std::mutex                    _mutex;
};
