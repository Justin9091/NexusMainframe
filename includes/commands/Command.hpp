//
// Created by jusra on 14-1-2026.
//

#ifndef NEXUSCORE_COMMAND_HPP
#define NEXUSCORE_COMMAND_HPP

#include <memory>
#include <string>
#include <vector>

class Command : public std::enable_shared_from_this<Command> {
public:
    Command();

    virtual ~Command() = default;

    virtual std::string getName() = 0;

    virtual std::string getDescription() = 0;

    virtual std::string execute(const std::vector<std::string> &args) = 0;
};

#endif //NEXUSCORE_COMMAND_HPP
