//
// Created by jusra on 15-1-2026.
//

#ifndef NEXUSCORE_SCANCOMMAND_HPP
#define NEXUSCORE_SCANCOMMAND_HPP
#include "Command.hpp"

class ScanCommand : public Command {
public:
    std::string getName() override;
    std::string getDescription() override;
    std::string execute(const std::vector<std::string> &args) override;
};

#endif //NEXUSCORE_SCANCOMMAND_HPP