#pragma once
#include <memory>
#include <optional>
#include "IService.hpp"
#include "devices/Device.hpp"
#include "devices/DeviceRegistry.hpp"
#include "Result/Result.hpp"
#include <nlohmann/json.hpp>

/**
 * @brief Service layer for device management.
 *
 * Owns the DeviceRegistry and exposes higher-level operations used by the HTTP
 * API and other services. Automatically generates a UUID on `add()` and
 * delegates persistence to the registry.
 *
 * @see DeviceRegistry, DeviceController
 */
class DeviceService : public IService {
public:
    std::string_view getName() const override { return "DeviceService"; }

    /** @brief Initializes the registry and loads persisted devices from disk. */
    void start() override;

    /** @brief Persists the current device state to disk. */
    void stop() override;

    /**
     * @brief Returns all registered devices.
     * @return Snapshot of the current device list.
     */
    std::vector<Device> list() const;

    /**
     * @brief Looks up a device by id.
     * @param id Device id to search for.
     * @return The device, or `std::nullopt` if not found.
     */
    std::optional<Device> get(const std::string& id) const;

    /**
     * @brief Creates and registers a new device.
     * @param name Human-readable name.
     * @param type Device type string, e.g. `"light"`.
     * @return Result indicating success or a descriptive error.
     */
    Result add(std::string name, std::string type);

    /**
     * @brief Removes a device by id.
     * @param id Device id to remove.
     * @return Result indicating success or a descriptive error.
     */
    Result remove(const std::string& id);

    /**
     * @brief Dispatches a command to a device.
     *
     * The @p payload schema depends on the device type. The command is forwarded
     * to the EventBus as a `device.command` event.
     *
     * @param id      Target device id.
     * @param payload JSON object describing the command, e.g. `{"action": "toggle"}`.
     * @return Result indicating success or a descriptive error.
     */
    Result command(const std::string& id, const nlohmann::json& payload);

    /**
     * @brief Direct access to the underlying registry.
     * @return Reference to the DeviceRegistry owned by this service.
     */
    DeviceRegistry& getRegistry() { return *_registry; }

private:
    std::unique_ptr<DeviceRegistry> _registry;
};
