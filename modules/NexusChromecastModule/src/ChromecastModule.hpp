#pragma once
#include <modules/IModule.hpp>
#include <event/EventBus.hpp>
#include "cast/CastDevice.hpp"
#include <map>
#include <mutex>
#include <vector>
#include <memory>

class ChromecastModule final : public IModule {
public:
    std::string getName() const override { return "ChromecastModule"; }
    void initialize(EventBus& eventBus) override;
    void shutdown() override;

private:
    void      handleCommand(const Event& event);
    CastDevice& getOrCreate(const std::string& host, int port);

    EventBus*           bus_ = nullptr;
    std::vector<int>    subIds_;
    std::map<std::string, std::unique_ptr<CastDevice>> devices_;
    std::mutex          mutex_;
};

#ifdef _WIN32
#define MODULE_EXPORT extern "C" __declspec(dllexport)
#else
#define MODULE_EXPORT extern "C"
#endif

MODULE_EXPORT IModule* createModule();
