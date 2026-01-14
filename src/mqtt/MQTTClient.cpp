#include "mqtt/MQTTClient.hpp"
#include <cstring>
#include <iostream>
#include <iomanip>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <unistd.h>
#endif

MQTTClient::MQTTClient(EventBus& bus, std::string id, std::string h, int p)
    : eventBus(bus), clientId(std::move(id)), host(std::move(h)), port(p) {}

MQTTClient::~MQTTClient() {
    disconnect();
}

bool MQTTClient::connect() {
    std::cout << "[MQTT] Connecting to " << host << ":" << port << "..." << std::endl;

#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        std::cout << "[MQTT] WSAStartup failed!" << std::endl;
        return false;
    }
#endif

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cout << "[MQTT] Socket creation failed!" << std::endl;
        return false;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(host.c_str());

    if (::connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cout << "[MQTT] Connection failed!" << std::endl;
        return false;
    }

    connected = true;
    std::cout << "[MQTT] Socket connected!" << std::endl;

    // MQTT CONNECT
    std::vector<uint8_t> packet;
    packet.push_back(0x10); // CONNECT

    std::vector<uint8_t> vh;
    vh.insert(vh.end(), {0x00,0x04,'M','Q','T','T'}); // protocol
    vh.push_back(0x04); // version
    vh.push_back(0x02); // clean session
    vh.insert(vh.end(), {0x00,0x3C}); // keepalive

    auto cid = encodeString(clientId);
    vh.insert(vh.end(), cid.begin(), cid.end());

    packet.push_back(static_cast<uint8_t>(vh.size()));
    packet.insert(packet.end(), vh.begin(), vh.end());

    std::cout << "[MQTT] Sending CONNECT packet (client: " << clientId << ")..." << std::endl;
    sendPacket(packet);

    running = true;
    rxThread = std::thread(&MQTTClient::receiveLoop, this);

    std::cout << "[MQTT] Connected! Receive thread started." << std::endl;
    return true;
}

bool MQTTClient::isConnected() {
    return connected;
}

void MQTTClient::disconnect() {
    std::cout << "[MQTT] Disconnecting..." << std::endl;
    running = false;
    if (rxThread.joinable()) rxThread.join();

#ifdef _WIN32
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif
    connected = false;
    std::cout << "[MQTT] Disconnected." << std::endl;
}

bool MQTTClient::subscribe(const std::string& topic) {
    static uint16_t pid = 1;

    std::cout << "[MQTT] ========================================" << std::endl;
    std::cout << "[MQTT] SUBSCRIBING TO: " << topic << std::endl;
    std::cout << "[MQTT] ========================================" << std::endl;

    std::vector<uint8_t> pkt;
    pkt.push_back(0x82); // SUBSCRIBE

    auto topicEnc = encodeString(topic);
    std::vector<uint8_t> body;
    body.push_back(pid >> 8);
    body.push_back(pid & 0xFF);
    body.insert(body.end(), topicEnc.begin(), topicEnc.end());
    body.push_back(0x00); // QoS 0

    pkt.push_back(static_cast<uint8_t>(body.size()));
    pkt.insert(pkt.end(), body.begin(), body.end());

    std::cout << "[MQTT] Subscription packet: ";
    for (auto b : pkt) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b << " ";
    }
    std::cout << std::dec << std::endl;

    bool result = sendPacket(pkt);
    std::cout << "[MQTT] Subscribe sent: " << (result ? "SUCCESS" : "FAILED") << std::endl;

    pid++;
    return result;
}

bool MQTTClient::publish(const std::string& topic, const std::string& payload) {
    std::cout << "[MQTT] Publishing to " << topic << ": " << payload << std::endl;

    std::vector<uint8_t> pkt;
    pkt.push_back(0x30); // PUBLISH QoS0

    auto t = encodeString(topic);
    std::vector<uint8_t> body = t;
    body.insert(body.end(), payload.begin(), payload.end());

    pkt.push_back(static_cast<uint8_t>(body.size()));
    pkt.insert(pkt.end(), body.begin(), body.end());

    return sendPacket(pkt);
}

void MQTTClient::receiveLoop() {
    std::cout << "[MQTT] Receive loop started, waiting for messages..." << std::endl;

    uint8_t buf[1024];
    while (running) {
        int n = recv(sock, reinterpret_cast<char*>(buf), sizeof(buf), 0);
        if (n > 0) {
            std::cout << "[MQTT] ✓✓✓ PACKET RECEIVED! " << n << " bytes ✓✓✓" << std::endl;  // <-- Extra visible!

            // Print hex dump
            std::cout << "[MQTT] Raw data: ";
            for (int i = 0; i < n && i < 32; i++) {
                std::cout << std::hex << std::setw(2) << std::setfill('0')
                          << (int)buf[i] << " ";
            }
            if (n > 32) std::cout << "...";
            std::cout << std::dec << std::endl;

            handlePacket({buf, buf + n});
        } else if (n == 0) {
            std::cout << "[MQTT] Connection closed by server" << std::endl;
            connected = false;
            break;
        } else {
            // On Windows, check for WSAEWOULDBLOCK
#ifdef _WIN32
            int err = WSAGetLastError();
            if (err == WSAEWOULDBLOCK || err == WSAETIMEDOUT) {
                // Timeout is normal, just continue
                continue;
            }
#else
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }
#endif
            std::cout << "[MQTT] Receive error" << std::endl;
            break;
        }
    }

    std::cout << "[MQTT] Receive loop ended." << std::endl;
}

void MQTTClient::handlePacket(const std::vector<uint8_t>& data) {
    if (data.empty()) {
        std::cout << "[MQTT] Empty packet received" << std::endl;
        return;
    }

    uint8_t type = data[0] >> 4;

    std::cout << "[MQTT] =====================================" << std::endl;
    std::cout << "[MQTT] Packet type: " << (int)type;

    switch(type) {
        case 2: std::cout << " (CONNACK)"; break;
        case 3: std::cout << " (PUBLISH)"; break;
        case 4: std::cout << " (PUBACK)"; break;
        case 9: std::cout << " (SUBACK)"; break;
        case 13: std::cout << " (PINGRESP)"; break;
        default: std::cout << " (Unknown)"; break;
    }
    std::cout << std::endl;

    // Print complete hex dump voor debugging
    std::cout << "[MQTT] Full packet hex: ";
    for (size_t i = 0; i < data.size(); i++) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << (int)data[i] << " ";
    }
    std::cout << std::dec << std::endl;

    if (type == 3) { // PUBLISH
        size_t pos = 1; // Start na type byte

        // Parse remaining length (simpel, gaat uit van 1 byte)
        uint8_t remaining_length = data[pos];
        pos++;

        std::cout << "[MQTT] Remaining length: " << (int)remaining_length << std::endl;

        if (pos + 2 > data.size()) {
            std::cout << "[MQTT] Invalid PUBLISH packet (too short for topic length)" << std::endl;
            return;
        }

        // Parse topic length
        uint16_t topic_len = (data[pos] << 8) | data[pos+1];
        pos += 2;

        std::cout << "[MQTT] Topic length: " << topic_len << std::endl;

        if (pos + topic_len > data.size()) {
            std::cout << "[MQTT] Invalid PUBLISH packet (topic length exceeds packet size)" << std::endl;
            std::cout << "[MQTT] Position: " << pos << ", Topic length: " << topic_len
                      << ", Data size: " << data.size() << std::endl;
            return;
        }

        // Extract topic
        std::string topic(reinterpret_cast<const char*>(&data[pos]), topic_len);
        pos += topic_len;

        std::cout << "[MQTT] Payload starts at position: " << pos << std::endl;
        std::cout << "[MQTT] Remaining bytes for payload: " << (data.size() - pos) << std::endl;

        // Extract payload (rest van het packet)
        std::string payload;
        if (pos < data.size()) {
            payload = std::string(reinterpret_cast<const char*>(&data[pos]), data.size() - pos);
        }

        std::cout << "[MQTT] =====================================" << std::endl;
        std::cout << "[MQTT] 📨 MESSAGE RECEIVED" << std::endl;
        std::cout << "[MQTT] =====================================" << std::endl;
        std::cout << "[MQTT] Topic:   \"" << topic << "\"" << std::endl;
        std::cout << "[MQTT] Payload: \"" << payload << "\"" << std::endl;
        std::cout << "[MQTT] =====================================" << std::endl;

        // Publish to event bus
        eventBus.publish(Event{
            .name = "mqtt:" + topic,
            .data = payload
        });

        std::cout << "[MQTT] ✓ Event published to bus: mqtt:" << topic << std::endl;
    } else if (type == 2) { // CONNACK
        std::cout << "[MQTT] ✓ Connection acknowledged by broker" << std::endl;
    } else if (type == 9) { // SUBACK
        std::cout << "[MQTT] ✓ Subscription acknowledged" << std::endl;
    }

    std::cout << "[MQTT] =====================================" << std::endl;
}

bool MQTTClient::sendPacket(const std::vector<uint8_t>& packet) {
    int sent = send(sock, reinterpret_cast<const char*>(packet.data()), packet.size(), 0);
    std::cout << "[MQTT] Sent " << sent << " bytes" << std::endl;
    return sent > 0;
}

std::vector<uint8_t> MQTTClient::encodeString(const std::string& s) {
    std::vector<uint8_t> out;
    out.reserve(2 + s.size());

    out.push_back(static_cast<uint8_t>((s.size() >> 8) & 0xFF));
    out.push_back(static_cast<uint8_t>(s.size() & 0xFF));
    out.insert(out.end(), s.begin(), s.end());

    return out;
}