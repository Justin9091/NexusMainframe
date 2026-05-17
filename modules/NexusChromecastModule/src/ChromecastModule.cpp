#include "ChromecastModule.hpp"
#include <nlohmann/json.hpp>
#include <any>

using json = nlohmann::json;

// ── IModule ───────────────────────────────────────────────────────────────────

void ChromecastModule::initialize(EventBus& eventBus) {
    bus_ = &eventBus;

    int id = eventBus.subscribe("chromecast.command", [this](const Event& e) {
        handleCommand(e);
    });
    subIds_.push_back(id);

    getLogger().logInfo("ChromecastModule initialized — listening on chromecast.command");
}

void ChromecastModule::shutdown() {
    if (bus_) {
        for (int id : subIds_)
            bus_->unsubscribe("chromecast.command", id);
        subIds_.clear();
    }

    std::lock_guard lock(mutex_);
    for (auto& [key, dev] : devices_)
        dev->disconnect();
    devices_.clear();

    getLogger().logInfo("ChromecastModule shut down");
}

// ── Device management ─────────────────────────────────────────────────────────

CastDevice& ChromecastModule::getOrCreate(const std::string& host, int port) {
    // Caller must hold mutex_
    std::string key = host + ":" + std::to_string(port);
    auto it = devices_.find(key);
    if (it != devices_.end()) return *it->second;

    auto dev = std::make_unique<CastDevice>(host, port);

    dev->onStatus([this, host](const CastDevice::ReceiverStatus& s) {
        if (!bus_) return;
        bus_->publish({"chromecast.status", json{
            {"host",         host},
            {"sessionId",    s.sessionId},
            {"transportId",  s.transportId},
            {"appId",        s.appId},
            {"displayName",  s.displayName},
            {"isIdleScreen", s.isIdleScreen},
            {"volume",       s.volumeLevel},
            {"muted",        s.muted}
        }.dump()});
    });

    dev->onMedia([this, host](const CastDevice::MediaStatus& m) {
        if (!bus_) return;
        bus_->publish({"chromecast.media", json{
            {"host",           host},
            {"mediaSessionId", m.sessionId},
            {"playerState",    m.playerState},
            {"currentTime",    m.currentTime},
            {"duration",       m.duration},
            {"title",          m.title},
            {"contentId",      m.contentId},
            {"contentType",    m.contentType}
        }.dump()});
    });

    dev->onError([this, host](const std::string& err) {
        getLogger().logError("[" + host + "] " + err);
        if (bus_)
            bus_->publish({"chromecast.error", json{{"host", host}, {"error", err}}.dump()});
    });

    auto& ref = *dev;
    devices_[key] = std::move(dev);
    return ref;
}

// ── Command handler ───────────────────────────────────────────────────────────
//
// Expected event data: JSON string
// {
//   "host":        "192.168.x.x",          (required)
//   "port":        8009,                    (optional, default 8009)
//   "action":      "<action>",              (required)
//
//   // action-specific fields:
//   "url":         "...",                   // loadMedia
//   "contentType": "video/mp4",             // loadMedia
//   "title":       "...",                   // loadMedia (optional)
//   "seconds":     42.0,                    // seek
//   "level":       0.8,                     // setVolume  (0.0–1.0)
//   "muted":       true                     // setMuted
// }
//
// Supported actions:
//   connect | disconnect | getStatus | launchMediaReceiver | stopApp
//   loadMedia | play | pause | stopMedia | seek | setVolume | setMuted

void ChromecastModule::handleCommand(const Event& event) {
    std::string payload;
    try {
        payload = std::any_cast<std::string>(event.data);
    } catch (...) {
        getLogger().logError("chromecast.command: payload must be a JSON string");
        return;
    }

    json cmd;
    try { cmd = json::parse(payload); }
    catch (...) { getLogger().logError("chromecast.command: invalid JSON"); return; }

    std::string host   = cmd.value("host",   "");
    int         port   = cmd.value("port",   8009);
    std::string action = cmd.value("action", "");

    if (host.empty() || action.empty()) {
        getLogger().logWarning("chromecast.command: 'host' and 'action' are required");
        return;
    }

    std::lock_guard lock(mutex_);
    auto& device = getOrCreate(host, port);

    if      (action == "connect")             { if (!device.isConnected()) device.connect(); }
    else if (action == "disconnect")          { device.disconnect(); }
    else if (action == "getStatus")           { device.getStatus(); }
    else if (action == "launchMediaReceiver") { device.launchDefaultMediaReceiver(); }
    else if (action == "stopApp")             { device.stopApp(); }
    else if (action == "play")                { device.play(); }
    else if (action == "pause")               { device.pause(); }
    else if (action == "stopMedia")           { device.stopMedia(); }
    else if (action == "seek")                { device.seek(cmd.value("seconds", 0.0)); }
    else if (action == "setVolume")           { device.setVolume(cmd.value("level", 1.0)); }
    else if (action == "setMuted")            { device.setMuted(cmd.value("muted", false)); }
    else if (action == "loadMedia") {
        std::string url  = cmd.value("url", "");
        if (url.empty()) { getLogger().logWarning("loadMedia: 'url' is required"); return; }
        device.loadMedia(url,
                         cmd.value("contentType", "video/mp4"),
                         cmd.value("title",       ""));
    }
    else {
        getLogger().logWarning("chromecast.command: unknown action '" + action + "'");
    }
}

// ── Module entry point ────────────────────────────────────────────────────────

MODULE_EXPORT IModule* createModule() {
    return new ChromecastModule();
}
