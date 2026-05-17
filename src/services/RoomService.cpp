#include "services/RoomService.hpp"
#include "services/DeviceService.hpp"
#include "pathing/PathManager.hpp"
#include <random>
#include <sstream>
#include <iomanip>

static std::string generateRoomId() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;
    std::ostringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << dis(gen);
    return ss.str();
}

RoomService::RoomService(DeviceService& devices)
    : _devices(devices) {}

void RoomService::start() {
    auto& paths = PathManager::getInstance();
    _registry = std::make_unique<RoomRegistry>(paths.get("data.rooms"));
    _registry->load();
}

void RoomService::stop() {
    if (_registry) _registry->save();
}

std::vector<Room> RoomService::list() const {
    return _registry->list();
}

std::optional<Room> RoomService::get(const std::string& id) const {
    return _registry->get(id);
}

Result RoomService::add(std::string name) {
    if (name.empty())
        return Result::badRequest("Field 'name' is required");

    Room r;
    r.id   = generateRoomId();
    r.name = std::move(name);

    if (!_registry->add(r))
        return Result::conflict("Room with this id already exists");

    _registry->save();
    return Result::created("Room '" + r.id + "' created");
}

Result RoomService::remove(const std::string& id) {
    if (!_registry->exists(id))
        return Result::notFound("Room '" + id + "' not found");

    _registry->remove(id);
    _registry->save();
    return Result::ok("Room '" + id + "' removed");
}

Result RoomService::addDevice(const std::string& roomId, const std::string& deviceId) {
    if (!_registry->exists(roomId))
        return Result::notFound("Room '" + roomId + "' not found");
    if (!_devices.getRegistry().exists(deviceId))
        return Result::notFound("Device '" + deviceId + "' not found");
    if (!_registry->addDevice(roomId, deviceId))
        return Result::conflict("Device already in this room");
    _registry->save();
    return Result::ok("Device added to room");
}

Result RoomService::removeDevice(const std::string& roomId, const std::string& deviceId) {
    if (!_registry->exists(roomId))
        return Result::notFound("Room '" + roomId + "' not found");
    if (!_registry->removeDevice(roomId, deviceId))
        return Result::notFound("Device not in this room");
    _registry->save();
    return Result::ok("Device removed from room");
}

nlohmann::json RoomService::expand(const Room& room) const {
    nlohmann::json j = room.toJson();
    nlohmann::json devicesArr = nlohmann::json::array();
    for (const auto& did : room.deviceIds) {
        auto d = _devices.getRegistry().get(did);
        if (d) devicesArr.push_back(d->toJson());
    }
    j["devices"] = devicesArr;
    return j;
}
