#include "CastDevice.hpp"

using json = nlohmann::json;

static const char* NS_RECEIVER = "urn:x-cast:com.google.cast.receiver";
static const char* NS_MEDIA    = "urn:x-cast:com.google.cast.media";
static const char* APP_DMR     = "CC1AD845"; // Default Media Receiver
static const char* SENDER_ID   = "sender-0";
static const char* RECEIVER_ID = "receiver-0";

CastDevice::CastDevice(std::string host, int port)
    : host_(std::move(host)), port_(port) {}

CastDevice::~CastDevice() { disconnect(); }

bool CastDevice::connect() {
    channel_ = std::make_unique<CastChannel>(host_, port_);

    channel_->onMessage([this](const CastMessage& m) { handleMessage(m); });
    channel_->onDisconnect([this] {
        if (errorCallback_) errorCallback_("Disconnected from " + host_);
    });

    if (!channel_->connect()) return false;
    if (!channel_->sendConnect()) return false;

    getStatus();
    return true;
}

void CastDevice::disconnect() {
    if (!channel_) return;
    channel_->disconnect();
    channel_.reset();
    std::lock_guard lock(mutex_);
    sessionId_.clear();
    transportId_.clear();
    mediaSessionId_ = -1;
}

bool CastDevice::isConnected() const {
    return channel_ && channel_->isConnected();
}

std::optional<CastDevice::ReceiverStatus> CastDevice::lastStatus() const {
    std::lock_guard lock(mutex_);
    return lastStatus_;
}

std::optional<CastDevice::MediaStatus> CastDevice::lastMediaStatus() const {
    std::lock_guard lock(mutex_);
    return lastMediaStatus_;
}

// ── Receiver control ──────────────────────────────────────────────────────────

void CastDevice::getStatus() {
    sendReceiverMsg({{"type", "GET_STATUS"}, {"requestId", nextId()}});
}

void CastDevice::launchDefaultMediaReceiver() {
    sendReceiverMsg({
        {"type", "LAUNCH"},
        {"appId", APP_DMR},
        {"requestId", nextId()}
    });
}

void CastDevice::stopApp() {
    std::string sid;
    { std::lock_guard lock(mutex_); sid = sessionId_; }
    if (sid.empty()) return;
    sendReceiverMsg({{"type", "STOP"}, {"sessionId", sid}, {"requestId", nextId()}});
}

void CastDevice::setVolume(double level) {
    sendReceiverMsg({
        {"type", "SET_VOLUME"},
        {"volume", {{"level", level}}},
        {"requestId", nextId()}
    });
}

void CastDevice::setMuted(bool muted) {
    sendReceiverMsg({
        {"type", "SET_VOLUME"},
        {"volume", {{"muted", muted}}},
        {"requestId", nextId()}
    });
}

// ── Media control ─────────────────────────────────────────────────────────────

void CastDevice::loadMedia(const std::string& url,
                           const std::string& contentType,
                           const std::string& title) {
    std::string transport;
    { std::lock_guard lock(mutex_); transport = transportId_; }
    if (transport.empty()) return;

    // Connect sender to the app's transport
    channel_->sendConnect(SENDER_ID, transport);

    sendMediaMsg({
        {"type", "LOAD"},
        {"requestId", nextId()},
        {"autoplay", true},
        {"currentTime", 0},
        {"media", {
            {"contentId", url},
            {"contentType", contentType},
            {"streamType", "BUFFERED"},
            {"metadata", {
                {"metadataType", 0},
                {"title", title.empty() ? url : title}
            }}
        }}
    });
}

void CastDevice::play() {
    int mid; std::string transport;
    { std::lock_guard lock(mutex_); mid = mediaSessionId_; transport = transportId_; }
    if (transport.empty() || mid < 0) return;
    sendMediaMsg({{"type", "PLAY"}, {"requestId", nextId()}, {"mediaSessionId", mid}});
}

void CastDevice::pause() {
    int mid; std::string transport;
    { std::lock_guard lock(mutex_); mid = mediaSessionId_; transport = transportId_; }
    if (transport.empty() || mid < 0) return;
    sendMediaMsg({{"type", "PAUSE"}, {"requestId", nextId()}, {"mediaSessionId", mid}});
}

void CastDevice::stopMedia() {
    int mid; std::string transport;
    { std::lock_guard lock(mutex_); mid = mediaSessionId_; transport = transportId_; }
    if (transport.empty() || mid < 0) return;
    sendMediaMsg({{"type", "STOP"}, {"requestId", nextId()}, {"mediaSessionId", mid}});
}

void CastDevice::seek(double seconds) {
    int mid; std::string transport;
    { std::lock_guard lock(mutex_); mid = mediaSessionId_; transport = transportId_; }
    if (transport.empty() || mid < 0) return;
    sendMediaMsg({
        {"type", "SEEK"},
        {"requestId", nextId()},
        {"mediaSessionId", mid},
        {"currentTime", seconds}
    });
}

// ── Message dispatch ──────────────────────────────────────────────────────────

void CastDevice::handleMessage(const CastMessage& msg) {
    try {
        auto j = json::parse(msg.payloadUtf8);
        if (msg.namespaceUri == NS_RECEIVER) handleReceiverStatus(j);
        else if (msg.namespaceUri == NS_MEDIA) handleMediaStatus(j);
    } catch (...) {}
}

void CastDevice::handleReceiverStatus(const json& j) {
    if (j.value("type", "") != "RECEIVER_STATUS") return;

    const auto& s = j["status"];
    ReceiverStatus status;

    if (s.contains("volume")) {
        status.volumeLevel = s["volume"].value("level", 1.0);
        status.muted       = s["volume"].value("muted",  false);
    }

    if (s.contains("applications") && !s["applications"].empty()) {
        const auto& app = s["applications"][0];
        status.sessionId   = app.value("sessionId",   "");
        status.transportId = app.value("transportId", "");
        status.appId       = app.value("appId",       "");
        status.displayName = app.value("displayName", "");
        status.isIdleScreen= app.value("isIdleScreen", false);
    } else {
        status.isIdleScreen = true;
    }

    {
        std::lock_guard lock(mutex_);
        lastStatus_   = status;
        sessionId_    = status.sessionId;
        transportId_  = status.transportId;
    }

    if (statusCallback_) statusCallback_(status);
}

void CastDevice::handleMediaStatus(const json& j) {
    if (j.value("type", "") != "MEDIA_STATUS") return;
    if (!j.contains("status") || j["status"].empty()) return;

    const auto& ms = j["status"][0];
    MediaStatus status;
    status.sessionId   = ms.value("mediaSessionId", -1);
    status.playerState = ms.value("playerState",    "UNKNOWN");
    status.currentTime = ms.value("currentTime",    0.0);

    if (ms.contains("media")) {
        const auto& m = ms["media"];
        status.contentId   = m.value("contentId",   "");
        status.contentType = m.value("contentType", "");
        status.duration    = m.value("duration",    0.0);
        if (m.contains("metadata"))
            status.title = m["metadata"].value("title", "");
    }

    {
        std::lock_guard lock(mutex_);
        lastMediaStatus_ = status;
        mediaSessionId_  = status.sessionId;
    }

    if (mediaCallback_) mediaCallback_(status);
}

// ── Helpers ───────────────────────────────────────────────────────────────────

void CastDevice::sendReceiverMsg(const json& payload) {
    if (!channel_) return;
    CastMessage msg;
    msg.sourceId      = SENDER_ID;
    msg.destinationId = RECEIVER_ID;
    msg.namespaceUri  = NS_RECEIVER;
    msg.payloadUtf8   = payload.dump();
    channel_->send(msg);
}

void CastDevice::sendMediaMsg(const json& payload) {
    std::string transport;
    { std::lock_guard lock(mutex_); transport = transportId_; }
    if (!channel_ || transport.empty()) return;

    CastMessage msg;
    msg.sourceId      = SENDER_ID;
    msg.destinationId = transport;
    msg.namespaceUri  = NS_MEDIA;
    msg.payloadUtf8   = payload.dump();
    channel_->send(msg);
}
