//
// Created by jusra on 14-1-2026.
//

#ifndef NEXUSCORE_LISTCOMMAND_HPP
#define NEXUSCORE_LISTCOMMAND_HPP
#include "Command.hpp"
#include "Modules/ModuleLoader.hpp"

class ListCommand : public Command {
private:
    ModuleLoader* _moduleLoader;

public:
    ListCommand(ModuleLoader* moduleLoader);
    std::string getName() override;
    std::string getDescription() override;

    std::string execute(const std::vector<std::string> &args) override;
};

#endif //NEXUSCORE_LISTCOMMAND_HPP