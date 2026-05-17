#include "services/DeviceService.hpp"
#include "pathing/PathManager.hpp"
#include "Event/EventBus.hpp"
#include <random>
#include <sstream>
#include <iomanip>

static std::string generateId() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;
    std::ostringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << dis(gen);
    return ss.str();
}

void DeviceService::start() {
    auto& paths = PathManager::getInstance();
    _registry = std::make_unique<DeviceRegistry>(paths.get("data.devices"));
    _registry->load();
}

void DeviceService::stop() {
    if (_registry) _registry->save();
}

std::vector<Device> DeviceService::list() const {
    return _registry->list();
}

std::optional<Device> DeviceService::get(const std::string& id) const {
    return _registry->get(id);
}

Result DeviceService::add(std::string name, std::string type) {
    if (name.empty() || type.empty())
        return Result::badRequest("Fields 'name' and 'type' are required");

    Device d;
    d.id   = generateId();
    d.name = std::move(name);
    d.type = std::move(type);

    if (!_registry->add(d))
        return Result::conflict("Device with this id already exists");

    _registry->save();

    EventBus::getInstance().publish({"device.registered", d.id});
    return Result::created("Device '" + d.id + "' registered");
}

Result DeviceService::remove(const std::string& id) {
    if (!_registry->exists(id))
        return Result::notFound("Device '" + id + "' not found");

    _registry->remove(id);
    _registry->save();

    EventBus::getInstance().publish({"device.removed", id});
    return Result::ok("Device '" + id + "' removed");
}

Result DeviceService::command(const std::string& id, const nlohmann::json& payload) {
    if (!_registry->exists(id))
        return Result::notFound("Device '" + id + "' not found");

    _registry->updateState(id, payload);
    _registry->save();

    EventBus::getInstance().publish({"device.command", nlohmann::json{{"id", id}, {"payload", payload}}.dump()});
    return Result::ok("Command applied to device '" + id + "'");
}
