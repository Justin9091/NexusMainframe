#pragma once
#include <filesystem>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "Rooms/Room.hpp"

class RoomRegistry {
public:
    explicit RoomRegistry(std::filesystem::path persistPath);

    void load();
    void save() const;

    bool                 add(Room room);
    std::optional<Room>  get(const std::string& id) const;
    std::vector<Room>    list() const;
    bool                 exists(const std::string& id) const;
    bool                 remove(const std::string& id);
    bool                 addDevice(const std::string& roomId, const std::string& deviceId);
    bool                 removeDevice(const std::string& roomId, const std::string& deviceId);

private:
    std::filesystem::path                  _path;
    std::unordered_map<std::string, Room>  _rooms;
    mutable std::mutex                     _mutex;
};
