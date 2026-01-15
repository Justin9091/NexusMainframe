//
// Created by jusra on 14-1-2026.
//

#ifndef NEXUSCORE_MANIFFEST_HPP
#define NEXUSCORE_MANIFFEST_HPP
#include <string>
#include <unordered_set>

class Manifest {
private:
    std::unordered_set<std::string> _available;
    std::unordered_set<std::string> _enabled;

public:
    bool save();
    bool load();
    void listen();
};

#endif //NEXUSCORE_MANIFFEST_HPP