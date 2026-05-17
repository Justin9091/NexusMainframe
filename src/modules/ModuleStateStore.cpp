#include "Modules/ModuleStateStore.hpp"

#include <algorithm>
#include <fstream>
#include <nlohmann/json.hpp>

ModuleStateStore::ModuleStateStore(std::filesystem::path statePath)
    : path_(std::move(statePath)) {}

void ModuleStateStore::load() {
    if (!std::filesystem::exists(path_)) return;

    std::ifstream in(path_);
    if (!in.is_open()) return;

    try {
        nlohmann::json j;
        in >> j;
        names_.clear();
        for (const auto& name : j.value("enabled", nlohmann::json::array())) {
            names_.push_back(name.get<std::string>());
        }
    } catch (...) {
        names_.clear();
    }
}

void ModuleStateStore::save() const {
    std::ofstream out(path_);
    if (!out.is_open()) return;

    nlohmann::json j;
    j["enabled"] = names_;
    out << j.dump(4);
}

void ModuleStateStore::add(const std::string& name) {
    if (!contains(name))
        names_.push_back(name);
}

void ModuleStateStore::remove(const std::string& name) {
    names_.erase(std::remove(names_.begin(), names_.end(), name), names_.end());
}

bool ModuleStateStore::contains(const std::string& name) const {
    return std::find(names_.begin(), names_.end(), name) != names_.end();
}

std::vector<std::string> ModuleStateStore::getAll() const {
    return names_;
}
