#pragma once
#include "CastChannel.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <functional>
#include <optional>
#include <memory>
#include <mutex>
#include <atomic>

class CastDevice {
public:
    struct ReceiverStatus {
        std::string sessionId;
        std::string transportId;
        std::string appId;
        std::string displayName;
        bool   isIdleScreen = true;
        double volumeLevel  = 1.0;
        bool   muted        = false;
    };

    struct MediaStatus {
        int         sessionId   = -1;
        std::string playerState;     // IDLE | PLAYING | PAUSED | BUFFERING
        double      currentTime = 0.0;
        double      duration    = 0.0;
        std::string title;
        std::string contentId;
        std::string contentType;
    };

    using StatusCallback = std::function<void(const ReceiverStatus&)>;
    using MediaCallback  = std::function<void(const MediaStatus&)>;
    using ErrorCallback  = std::function<void(const std::string&)>;

    CastDevice(std::string host, int port = 8009);
    ~CastDevice();

    bool connect();
    void disconnect();
    bool isConnected() const;

    const std::string& host() const { return host_; }
    int                port() const { return port_; }

    // Receiver control
    void getStatus();
    void launchDefaultMediaReceiver();
    void stopApp();

    // Volume
    void setVolume(double level);
    void setMuted(bool muted);

    // Media (requires an active session; call launchDefaultMediaReceiver first)
    void loadMedia(const std::string& url,
                   const std::string& contentType,
                   const std::string& title = "");
    void play();
    void pause();
    void stopMedia();
    void seek(double seconds);

    void onStatus(StatusCallback cb) { statusCallback_ = std::move(cb); }
    void onMedia (MediaCallback  cb) { mediaCallback_  = std::move(cb); }
    void onError (ErrorCallback  cb) { errorCallback_  = std::move(cb); }

    std::optional<ReceiverStatus> lastStatus()      const;
    std::optional<MediaStatus>    lastMediaStatus() const;

private:
    void handleMessage(const CastMessage& msg);
    void handleReceiverStatus(const nlohmann::json& j);
    void handleMediaStatus(const nlohmann::json& j);
    void sendReceiverMsg(const nlohmann::json& payload);
    void sendMediaMsg(const nlohmann::json& payload);
    int  nextId() { return ++requestId_; }

    std::string host_;
    int         port_;
    std::unique_ptr<CastChannel> channel_;

    std::atomic<int> requestId_{0};

    mutable std::mutex            mutex_;
    std::optional<ReceiverStatus> lastStatus_;
    std::optional<MediaStatus>    lastMediaStatus_;
    std::string sessionId_;
    std::string transportId_;
    int         mediaSessionId_ = -1;

    StatusCallback statusCallback_;
    MediaCallback  mediaCallback_;
    ErrorCallback  errorCallback_;
};
