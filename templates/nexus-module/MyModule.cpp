#include "MyModule.hpp"

// ─── IModule implementatie ───────────────────────────────────────────────────

std::string MyModule::getName() const {
    // Dit is de naam die nexus gebruikt bij /api/modules en state-opslag.
    // Moet uniek zijn; gebruik dezelfde naam als je .dll-bestand.
    return "MyModule";
}

void MyModule::initialize(EventBus& eventBus) {
    bus_ = &eventBus;
    auto log = getLogger();

    log.logInfo("MyModule initializing...");

    // Abonneer op events die je module interessant vindt.
    // Bewaar het ID om later te kunnen unsubsciben in shutdown().
    subId_ = eventBus.subscribe("some.event", [this](const Event& e) {
        auto log = getLogger();

        // std::any_cast naar het type dat de publisher erin stopt.
        // Controleer altijd of het type klopt voordat je cast.
        // auto payload = std::any_cast<std::string>(e.data);

        log.logInfo("Received: " + e.name);
    });

    // Publiceer een event naar andere modules of de core.
    // eventBus.publish({ "my.module.ready", std::string{"ok"} });

    log.logInfo("MyModule initialized.");
}

void MyModule::shutdown() {
    auto log = getLogger();
    log.logInfo("MyModule shutting down...");

    if (bus_ && subId_ >= 0) {
        bus_->unsubscribe("some.event", subId_);
    }

    // Ruim resources op: threads stoppen, handles sluiten, etc.
}

// ─── Export — verplicht, exact deze signatuur ────────────────────────────────
// nexus roept createModule() aan via dlopen/LoadLibrary.
// Gebruik extern "C" om name-mangling te voorkomen.

extern "C" IModule* createModule() {
    return new MyModule();
}
