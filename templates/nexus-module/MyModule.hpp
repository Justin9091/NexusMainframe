#pragma once

#include "Modules/IModule.hpp"

// Hernoem deze class naar je module-naam.
class MyModule final : public IModule {
public:
    std::string getName() const override;
    void initialize(EventBus& eventBus) override;
    void shutdown() override;

private:
    EventBus* bus_ = nullptr;

    // Sla subscription-IDs op zodat je ze in shutdown() kunt opruimen.
    int subId_ = -1;
};
