#include "Devices/DeviceRegistry.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

DeviceRegistry::DeviceRegistry(std::filesystem::path persistPath)
    : _path(std::move(persistPath)) {}

void DeviceRegistry::load() {
    std::lock_guard lock(_mutex);
    if (!std::filesystem::exists(_path)) return;

    std::ifstream f(_path);
    if (!f.is_open()) return;

    try {
        auto j = nlohmann::json::parse(f);
        for (const auto& item : j)
            _devices[item["id"].get<std::string>()] = Device::fromJson(item);
    } catch (...) {}
}

void DeviceRegistry::save() const {
    std::lock_guard lock(_mutex);
    std::filesystem::create_directories(_path.parent_path());

    nlohmann::json arr = nlohmann::json::array();
    for (const auto& [_, d] : _devices)
        arr.push_back(d.toJson());

    std::ofstream f(_path);
    f << arr.dump(2);
}

bool DeviceRegistry::add(Device device) {
    std::lock_guard lock(_mutex);
    if (_devices.contains(device.id)) return false;
    _devices.emplace(device.id, std::move(device));
    return true;
}

std::optional<Device> DeviceRegistry::get(const std::string& id) const {
    std::lock_guard lock(_mutex);
    auto it = _devices.find(id);
    if (it == _devices.end()) return std::nullopt;
    return it->second;
}

std::vector<Device> DeviceRegistry::list() const {
    std::lock_guard lock(_mutex);
    std::vector<Device> result;
    result.reserve(_devices.size());
    for (const auto& [_, d] : _devices)
        result.push_back(d);
    return result;
}

bool DeviceRegistry::updateState(const std::string& id, nlohmann::json patch) {
    std::lock_guard lock(_mutex);
    auto it = _devices.find(id);
    if (it == _devices.end()) return false;
    it->second.state.merge_patch(patch);
    return true;
}

bool DeviceRegistry::exists(const std::string& id) const {
    std::lock_guard lock(_mutex);
    return _devices.contains(id);
}

bool DeviceRegistry::remove(const std::string& id) {
    std::lock_guard lock(_mutex);
    return _devices.erase(id) > 0;
}
