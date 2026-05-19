#pragma once
#include <memory>
#include <optional>
#include "IService.hpp"
#include "rooms/Room.hpp"
#include "rooms/RoomRegistry.hpp"
#include "services/DeviceService.hpp"
#include "result/Result.hpp"
#include <nlohmann/json.hpp>

/**
 * @brief IService that manages room creation, membership, and device assignment.
 *
 * Owns a RoomRegistry (persisted to @c ~/.nexus/rooms.json) and exposes
 * room operations to the REST API via RoomController.
 */
class RoomService : public IService {
public:
    /**
     * @param devices  DeviceService used by expand() to resolve device objects.
     */
    explicit RoomService(DeviceService& devices);

    std::string_view getName() const override { return "RoomService"; }

    /** @brief Initialises the RoomRegistry and loads persisted rooms. */
    void start() override;

    /** @brief No-op (registry is persisted on every mutation). */
    void stop() override;

    /** @brief Returns all registered rooms. */
    std::vector<Room> list() const;

    /**
     * @brief Retrieves a room by ID.
     * @param id Room UUID.
     * @return The Room, or std::nullopt if not found.
     */
    std::optional<Room> get(const std::string& id) const;

    /**
     * @brief Creates a new room with an auto-generated UUID.
     * @param name Human-readable room name.
     * @return Result::created() on success; Result::failed() on error.
     */
    Result add(std::string name);

    /**
     * @brief Deletes a room by ID.
     * @param id Room UUID.
     * @return Result::ok() on success; Result::notFound() if absent.
     */
    Result remove(const std::string& id);

    /**
     * @brief Assigns a device to a room.
     * @param roomId    Room UUID.
     * @param deviceId  Device ID to assign.
     * @return Result::ok() on success; Result::notFound() if either is absent.
     */
    Result addDevice(const std::string& roomId, const std::string& deviceId);

    /**
     * @brief Removes a device from a room.
     * @param roomId    Room UUID.
     * @param deviceId  Device ID to remove.
     * @return Result::ok() on success; Result::notFound() if not found.
     */
    Result removeDevice(const std::string& roomId, const std::string& deviceId);

    /**
     * @brief Returns a JSON object for the room with full device details embedded.
     * @param room  Room to expand.
     * @return JSON with @c id, @c name, and @c devices array (full device objects).
     */
    nlohmann::json expand(const Room& room) const;

private:
    DeviceService&                _devices;
    std::unique_ptr<RoomRegistry> _registry;
};
