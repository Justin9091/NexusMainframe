#pragma once
#include <filesystem>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "devices/Device.hpp"

/**
 * @brief Thread-safe store for Device objects, backed by a JSON file on disk.
 *
 * Call `load()` once on startup and `save()` after any mutation to persist
 * changes. All read and write methods are protected by an internal mutex.
 *
 * @see Device, DeviceService
 */
class DeviceRegistry {
public:
    /**
     * @brief Constructs the registry.
     * @param persistPath Path to the JSON file used for persistence (created if absent).
     */
    explicit DeviceRegistry(std::filesystem::path persistPath);

    /**
     * @brief Loads devices from the persist file into memory.
     * Silently succeeds if the file does not exist yet.
     */
    void load();

    /**
     * @brief Writes the current in-memory state to the persist file.
     */
    void save() const;

    /**
     * @brief Adds a device to the registry.
     * @param device Device to add (must have a unique `id`).
     * @return `true` on success, `false` if a device with the same id already exists.
     */
    bool add(Device device);

    /**
     * @brief Looks up a device by id.
     * @param id Device id to search for.
     * @return The device, or `std::nullopt` if not found.
     */
    std::optional<Device> get(const std::string& id) const;

    /**
     * @brief Returns all registered devices.
     * @return Snapshot of the current device list.
     */
    std::vector<Device> list() const;

    /**
     * @brief Merges a JSON patch into a device's `state` field.
     * @param id    Target device id.
     * @param patch JSON object whose keys are merged into the existing state.
     * @return `true` on success, `false` if the device was not found.
     */
    bool updateState(const std::string& id, nlohmann::json patch);

    /**
     * @brief Checks whether a device with the given id exists.
     * @param id Device id to check.
     * @return `true` if found.
     */
    bool exists(const std::string& id) const;

    /**
     * @brief Removes a device from the registry.
     * @param id Device id to remove.
     * @return `true` on success, `false` if the device was not found.
     */
    bool remove(const std::string& id);

private:
    std::filesystem::path                   _path;
    std::unordered_map<std::string, Device> _devices;
    mutable std::mutex                      _mutex;
};
