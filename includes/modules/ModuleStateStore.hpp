#ifndef NEXUSCORE_MODULESTATESTORE_HPP
#define NEXUSCORE_MODULESTATESTORE_HPP

#include <filesystem>
#include <string>
#include <vector>

/**
 * @brief Persists the set of enabled module names to disk.
 *
 * Stores a JSON array of module names so ModuleManager can restore the
 * previously active set after a restart.  The file is typically located at
 * @c ~/.nexus/modules/state.json.
 *
 * @code
 * ModuleStateStore store("/home/user/.nexus/modules/state.json");
 * store.load();
 * if (!store.contains("my-sensor")) {
 *     store.add("my-sensor");
 *     store.save();
 * }
 * @endcode
 */
class ModuleStateStore {
public:
    /**
     * @param statePath  Absolute path to the JSON state file.
     *                   The file need not exist yet; it is created on save().
     */
    explicit ModuleStateStore(std::filesystem::path statePath);

    /**
     * @brief Loads the module name list from the state file.
     * If the file does not exist, the list is left empty.
     */
    void load();

    /** @brief Writes the current name list to disk as a JSON array. */
    void save() const;

    /**
     * @brief Adds a module name to the list (no-op if already present).
     * @param name Module name to persist.
     */
    void add(const std::string& name);

    /**
     * @brief Removes a module name from the list (no-op if not found).
     * @param name Module name to remove.
     */
    void remove(const std::string& name);

    /**
     * @brief Checks whether a module name is in the list.
     * @param name Module name to look up.
     * @return @c true if present.
     */
    bool contains(const std::string& name) const;

    /** @brief Returns a copy of all persisted module names. */
    std::vector<std::string> getAll() const;

private:
    std::filesystem::path    path_;
    std::vector<std::string> names_;
};

#endif // NEXUSCORE_MODULESTATESTORE_HPP
