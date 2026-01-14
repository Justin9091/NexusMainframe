//
// Created by jusra on 14-1-2026.
//

#ifndef NEXUSCORE_COMMANDREGISTRY_HPP
#define NEXUSCORE_COMMANDREGISTRY_HPP
#include <unordered_map>

class CommandRegistry {
public:
    static CommandRegistry& getInstance() {
        static CommandRegistry instance;
        return instance;
    }

    void registerCommand(const std::string& name, std::unique_ptr<Command> cmd) {
        commands[name] = std::move(cmd);
    }

    Command* getCommand(const std::string& name) {
        auto it = commands.find(name);
        if (it != commands.end()) return it->second.get();
        return nullptr;
    }

    const std::unordered_map<std::string, std::unique_ptr<Command>>& getAllCommands() {
        return commands;
    }

private:
    std::unordered_map<std::string, std::unique_ptr<Command>> commands;
    CommandRegistry() = default;
    CommandRegistry(const CommandRegistry&) = delete;
    CommandRegistry& operator=(const CommandRegistry&) = delete;
};

#endif //NEXUSCORE_COMMANDREGISTRY_HPP