//
// Created by jusra on 15-1-2026.
//

#ifndef NEXUSCORE_IMODULESCANNER_HPP
#define NEXUSCORE_IMODULESCANNER_HPP

#include <string>
#include <vector>

struct ModuleInfo {
    std::string name;
    std::string filename;
    std::string path;
    bool isValid;
};

class IModuleScanner {
public:
    virtual ~IModuleScanner() = default;

    // Scan directory en inspecteer modules
    virtual std::vector<ModuleInfo> scanDirectory(const std::string& directory) = 0;

    // Check of een module bestaat
    virtual bool moduleExists(const std::string& modulePath) = 0;
};

#endif
