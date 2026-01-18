//
// Created by jusra on 14-1-2026.
//

#ifndef NEXUSCORE_LISTCOMMAND_HPP
#define NEXUSCORE_LISTCOMMAND_HPP
#include "Command.hpp"
#include "Modules/ModuleManager.hpp"

class ListCommand : public Command {
private:
    const ModuleManager& _moduleManager;

public:
    ListCommand(const ModuleManager& moduleManager);
    std::string getName() override;
    std::string getDescription() override;
    std::string execute(const std::vector<std::string> &args) override;
};

#endif //NEXUSCORE_LISTCOMMAND_HPP