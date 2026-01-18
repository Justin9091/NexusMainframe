//
// Created by jusra on 18-1-2026.
//

#ifndef NEXUSCORE_ISERVICEHOST_HPP
#define NEXUSCORE_ISERVICEHOST_HPP

#include <functional>
#include <string>

class IServiceHost {
public:
    using StartCallback = std::function<void()>;
    using StopCallback  = std::function<void()>;

    virtual ~IServiceHost() = default;

    virtual void run(
        StartCallback onStart,
        StopCallback onStop
    ) = 0;
};

#endif //NEXUSCORE_ISERVICEHOST_HPP