#ifndef NEXUSCORE_MODULESTATESTORE_HPP
#define NEXUSCORE_MODULESTATESTORE_HPP

#include <filesystem>
#include <string>
#include <vector>

class ModuleStateStore {
public:
    explicit ModuleStateStore(std::filesystem::path statePath);

    void load();
    void save() const;

    void add(const std::string& name);
    void remove(const std::string& name);
    bool contains(const std::string& name) const;
    std::vector<std::string> getAll() const;

private:
    std::filesystem::path path_;
    std::vector<std::string> names_;
};

#endif // NEXUSCORE_MODULESTATESTORE_HPP
