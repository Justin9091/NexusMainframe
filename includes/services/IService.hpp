#pragma once
#include <string_view>

class IService {
public:
    virtual ~IService() = default;
    virtual std::string_view getName() const = 0;
    virtual void start() = 0;
    virtual void update() {}
    virtual void stop() = 0;
};
