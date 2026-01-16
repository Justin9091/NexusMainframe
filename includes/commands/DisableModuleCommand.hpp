//
// Created by jusra on 14-1-2026.
//

#ifndef NEXUSCORE_DISABLEMODULECOMMAND_HPP
#define NEXUSCORE_DISABLEMODULECOMMAND_HPP
#include "Command.hpp"
#include "Event/EventBus.hpp"
#include "Modules/ModuleLoader.hpp"
#include "Modules/ModuleManager.hpp"

class DisableModuleCommand : public Command {
private:
    ModuleManager& _moduleManager;
public:
    DisableModuleCommand(ModuleManager &moduleManager);

    std::string getName() override;
    std::string getDescription() override;
    std::string execute(const std::vector<std::string> &args) override;
};

#endif //NEXUSCORE_DISABLEMODULECOMMAND_HPP