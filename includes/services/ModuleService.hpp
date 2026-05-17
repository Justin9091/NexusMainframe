#pragma once
#include <memory>
#include <string>
#include <vector>
#include "IService.hpp"
#include "Modules/ModuleManager.hpp"
#include "Result/Result.hpp"

class ModuleService : public IService {
public:
    struct ModuleInfo {
        std::string name;
        std::string path;
    };

    std::string_view getName() const override { return "ModuleService"; }
    void start() override;
    void stop() override;

    std::vector<ModuleInfo> list() const;
    bool   isLoaded(const std::string& name) const;
    Result enable  (const std::string& name);
    Result disable (const std::string& name);

    // Kept until SystemController handles health/metrics
    ModuleManager& getManager() { return *_manager; }

private:
    std::unique_ptr<ModuleManager> _manager;
};
