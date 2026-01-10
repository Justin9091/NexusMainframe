//
// Created by jusra on 29-12-2025.
//

#ifndef NEXUSMAINFRAME_IMODULE_HPP
#define NEXUSMAINFRAME_IMODULE_HPP

#include <string>

#include "Event/EventBus.hpp"
#include "Logger/Logger.hpp"

class IModule {
public:
    virtual ~IModule() = default;
    virtual std::string getName() const = 0;
    virtual void initialize(EventBus& eventBus) = 0;
    virtual void shutdown() = 0;

    Logger getLogger() const {
        Logger logger {getName()};
        return logger;
    }
};

#endif //NEXUSMAINFRAME_IMODULE_HPP