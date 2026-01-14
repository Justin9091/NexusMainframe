//
// Created by jusra on 14-1-2026.
//

#ifndef NEXUSCORE_HELPCOMMAND_HPP
#define NEXUSCORE_HELPCOMMAND_HPP
#include "Command.hpp"
#include "CommandRegistry.hpp"

class HelpCommand : public Command {
public:
    std::string getName() override;
    std::string getDescription() override;
    std::string execute(const std::vector<std::string> &args) override;
};


#endif //NEXUSCORE_HELPCOMMAND_HPP