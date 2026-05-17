#include "CastChannel.hpp"
#include "CastProto.hpp"
#include <nlohmann/json.hpp>
#include <chrono>
#include <thread>

using json = nlohmann::json;

static constexpr auto HEARTBEAT_INTERVAL = std::chrono::seconds(5);
static constexpr uint32_t MAX_MESSAGE_SIZE = 1'000'000;

static const char* NS_CONNECTION = "urn:x-cast:com.google.cast.tp.connection";
static const char* NS_HEARTBEAT  = "urn:x-cast:com.google.cast.tp.heartbeat";

CastChannel::CastChannel(std::string host, int port)
    : host_(std::move(host)), port_(port) {}

CastChannel::~CastChannel() {
    disconnect();
}

bool CastChannel::connect() {
    if (!socket_.connect(host_, port_)) return false;
    running_ = true;
    recvThread_      = std::thread([this] { receiveLoop(); });
    heartbeatThread_ = std::thread([this] { heartbeatLoop(); });
    return true;
}

void CastChannel::disconnect() {
    running_ = false;
    socket_.disconnect();
    if (recvThread_.joinable())      recvThread_.join();
    if (heartbeatThread_.joinable()) heartbeatThread_.join();
}

bool CastChannel::isConnected() const {
    return running_ && socket_.isConnected();
}

bool CastChannel::send(const CastMessage& msg) {
    std::lock_guard lock(sendMutex_);

    auto body = encodeCastMessage(msg);
    auto len  = static_cast<uint32_t>(body.size());

    uint8_t hdr[4] = {
        static_cast<uint8_t>((len >> 24) & 0xFF),
        static_cast<uint8_t>((len >> 16) & 0xFF),
        static_cast<uint8_t>((len >>  8) & 0xFF),
        static_cast<uint8_t>( len        & 0xFF)
    };

    std::vector<uint8_t> packet;
    packet.reserve(4 + body.size());
    packet.insert(packet.end(), hdr, hdr + 4);
    packet.insert(packet.end(), body.begin(), body.end());

    return socket_.sendAll(packet);
}

bool CastChannel::readMessage(CastMessage& msg) {
    uint8_t hdr[4];
    if (!socket_.recvAll(hdr, 4)) return false;

    uint32_t len = (static_cast<uint32_t>(hdr[0]) << 24) |
                   (static_cast<uint32_t>(hdr[1]) << 16) |
                   (static_cast<uint32_t>(hdr[2]) <<  8) |
                    static_cast<uint32_t>(hdr[3]);

    if (len == 0 || len > MAX_MESSAGE_SIZE) return false;

    std::vector<uint8_t> body(len);
    if (!socket_.recvAll(body.data(), len)) return false;

    auto decoded = decodeCastMessage(body.data(), body.size());
    if (!decoded) return false;

    msg = std::move(*decoded);
    return true;
}

void CastChannel::receiveLoop() {
    while (running_) {
        CastMessage msg;
        if (!readMessage(msg)) {
            if (running_) {
                running_ = false;
                if (disconnectCallback_) disconnectCallback_();
            }
            break;
        }

        // Automatically answer PING with PONG
        if (msg.namespaceUri == NS_HEARTBEAT) {
            try {
                if (json::parse(msg.payloadUtf8).value("type", "") == "PING") {
                    CastMessage pong;
                    pong.sourceId      = msg.destinationId;
                    pong.destinationId = msg.sourceId;
                    pong.namespaceUri  = NS_HEARTBEAT;
                    pong.payloadUtf8   = R"({"type":"PONG"})";
                    send(pong);
                    continue;
                }
            } catch (...) {}
        }

        if (messageCallback_) messageCallback_(msg);
    }
}

void CastChannel::heartbeatLoop() {
    // Poll in short ticks so we notice running_=false quickly
    while (running_) {
        auto deadline = std::chrono::steady_clock::now() + HEARTBEAT_INTERVAL;
        while (running_ && std::chrono::steady_clock::now() < deadline)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (running_) sendHeartbeat();
    }
}

bool CastChannel::sendConnect(const std::string& sourceId, const std::string& destId) {
    CastMessage msg;
    msg.sourceId      = sourceId;
    msg.destinationId = destId;
    msg.namespaceUri  = NS_CONNECTION;
    msg.payloadUtf8   = R"({"type":"CONNECT","userAgent":"NexusChromecastModule/1.0"})";
    return send(msg);
}

bool CastChannel::sendHeartbeat(const std::string& sourceId, const std::string& destId) {
    CastMessage msg;
    msg.sourceId      = sourceId;
    msg.destinationId = destId;
    msg.namespaceUri  = NS_HEARTBEAT;
    msg.payloadUtf8   = R"({"type":"PING"})";
    return send(msg);
}
