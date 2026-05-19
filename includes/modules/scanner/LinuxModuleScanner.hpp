//
// Created by jusra on 15-1-2026.
//

#ifndef NEXUSCORE_LINUXMODULESCANNER_HPP
#define NEXUSCORE_LINUXMODULESCANNER_HPP

#include "IModuleScanner.hpp"

#ifndef _WIN32
#include <filesystem>

namespace fs = std::filesystem;

/**
 * @brief Linux IModuleScanner implementation using std::filesystem.
 *
 * Recursively scans for @c .so files; does not validate the module binary.
 *
 * @note Only compiled on non-Windows platforms.
 */
class LinuxModuleScanner : public IModuleScanner {
public:
    std::vector<ModuleInfo> scanDirectory(const std::string& directory) override {
        std::vector<ModuleInfo> modules;

        if (!fs::exists(directory)) {
            return modules;
        }

        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".so") {
                ModuleInfo info;
                info.name = entry.path().filename().string();
                info.path = entry.path().string();
                modules.push_back(info);
            }
        }

        return modules;
    }

    bool moduleExists(const std::string& modulePath) override {
        return fs::exists(modulePath);
    }
};

#endif // !_WIN32

#endif //NEXUSCORE_LINUXMODULESCANNER_HPP