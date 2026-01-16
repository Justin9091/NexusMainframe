//
// Created by jusra on 14-1-2026.
//

#ifndef NEXUSCORE_ENABLEMODULECOMMAND_HPP
#define NEXUSCORE_ENABLEMODULECOMMAND_HPP
#include "Command.hpp"
#include "Modules/ModuleLoader.hpp"
#include "Modules/ModuleManager.hpp"

class EnableModuleCommand : public Command {
private:
    ModuleManager& _moduleManager;

public:
    EnableModuleCommand(ModuleManager& moduleManager);

    std::string getName() override;
    std::string getDescription() override;
    std::string execute(const std::vector<std::string> &args) override;
};

#endif //NEXUSCORE_ENABLEMODULECOMMAND_HPP