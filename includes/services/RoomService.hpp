#pragma once
#include <memory>
#include <optional>
#include "IService.hpp"
#include "Rooms/Room.hpp"
#include "Rooms/RoomRegistry.hpp"
#include "services/DeviceService.hpp"
#include "Result/Result.hpp"
#include <nlohmann/json.hpp>

class RoomService : public IService {
public:
    explicit RoomService(DeviceService& devices);

    std::string_view getName() const override { return "RoomService"; }
    void start() override;
    void stop() override;

    std::vector<Room>    list() const;
    std::optional<Room>  get(const std::string& id) const;
    Result               add(std::string name);
    Result               remove(const std::string& id);
    Result               addDevice(const std::string& roomId, const std::string& deviceId);
    Result               removeDevice(const std::string& roomId, const std::string& deviceId);

    // Returns room with full device objects embedded
    nlohmann::json       expand(const Room& room) const;

private:
    DeviceService&                   _devices;
    std::unique_ptr<RoomRegistry>    _registry;
};
