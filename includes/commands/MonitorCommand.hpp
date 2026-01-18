//
// Created by jusra on 16-1-2026.
//

#ifndef NEXUSCORE_MONITORCOMMAND_HPP
#define NEXUSCORE_MONITORCOMMAND_HPP

#include "commands/Command.hpp"
#include <string>
#include <vector>

class MonitorCommand : public Command {
public:
    MonitorCommand() = default;
    ~MonitorCommand() override = default;
    std::string getName() override;
    std::string getDescription() override;
    std::string execute(const std::vector<std::string> &args) override;

private:
    std::string getMemoryUsage();
    std::string getCPUUsage();
    std::string generateMonitorOutput();
};

#endif //NEXUSCORE_MONITORCOMMAND_HPP