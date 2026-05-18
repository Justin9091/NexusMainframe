#include "rooms/RoomRegistry.hpp"
#include <fstream>
#include <algorithm>

RoomRegistry::RoomRegistry(std::filesystem::path persistPath)
    : _path(std::move(persistPath)) {}

void RoomRegistry::load() {
    std::lock_guard lock(_mutex);
    if (!std::filesystem::exists(_path)) return;

    std::ifstream f(_path);
    if (!f.is_open()) return;

    try {
        auto j = nlohmann::json::parse(f);
        for (const auto& item : j)
            _rooms[item["id"].get<std::string>()] = Room::fromJson(item);
    } catch (...) {}
}

void RoomRegistry::save() const {
    std::lock_guard lock(_mutex);
    std::filesystem::create_directories(_path.parent_path());

    nlohmann::json arr = nlohmann::json::array();
    for (const auto& [_, r] : _rooms)
        arr.push_back(r.toJson());

    std::ofstream f(_path);
    f << arr.dump(2);
}

bool RoomRegistry::add(Room room) {
    std::lock_guard lock(_mutex);
    if (_rooms.contains(room.id)) return false;
    _rooms.emplace(room.id, std::move(room));
    return true;
}

std::optional<Room> RoomRegistry::get(const std::string& id) const {
    std::lock_guard lock(_mutex);
    auto it = _rooms.find(id);
    if (it == _rooms.end()) return std::nullopt;
    return it->second;
}

std::vector<Room> RoomRegistry::list() const {
    std::lock_guard lock(_mutex);
    std::vector<Room> result;
    result.reserve(_rooms.size());
    for (const auto& [_, r] : _rooms)
        result.push_back(r);
    return result;
}

bool RoomRegistry::exists(const std::string& id) const {
    std::lock_guard lock(_mutex);
    return _rooms.contains(id);
}

bool RoomRegistry::remove(const std::string& id) {
    std::lock_guard lock(_mutex);
    return _rooms.erase(id) > 0;
}

bool RoomRegistry::addDevice(const std::string& roomId, const std::string& deviceId) {
    std::lock_guard lock(_mutex);
    auto it = _rooms.find(roomId);
    if (it == _rooms.end()) return false;
    auto& ids = it->second.deviceIds;
    if (std::find(ids.begin(), ids.end(), deviceId) != ids.end()) return false;
    ids.push_back(deviceId);
    return true;
}

bool RoomRegistry::removeDevice(const std::string& roomId, const std::string& deviceId) {
    std::lock_guard lock(_mutex);
    auto it = _rooms.find(roomId);
    if (it == _rooms.end()) return false;
    auto& ids = it->second.deviceIds;
    auto pos = std::find(ids.begin(), ids.end(), deviceId);
    if (pos == ids.end()) return false;
    ids.erase(pos);
    return true;
}
