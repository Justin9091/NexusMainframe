#include "CastProto.hpp"

// ── Encoder ──────────────────────────────────────────────────────────────────

static void writeVarint(std::vector<uint8_t>& buf, uint64_t v) {
    while (v > 0x7F) {
        buf.push_back(static_cast<uint8_t>((v & 0x7F) | 0x80));
        v >>= 7;
    }
    buf.push_back(static_cast<uint8_t>(v));
}

static void writeTag(std::vector<uint8_t>& buf, int field, int wireType) {
    writeVarint(buf, (static_cast<uint64_t>(field) << 3) | wireType);
}

static void writeVarintField(std::vector<uint8_t>& buf, int field, uint64_t v) {
    writeTag(buf, field, 0);
    writeVarint(buf, v);
}

static void writeStringField(std::vector<uint8_t>& buf, int field, const std::string& s) {
    writeTag(buf, field, 2);
    writeVarint(buf, s.size());
    buf.insert(buf.end(), s.begin(), s.end());
}

std::vector<uint8_t> encodeCastMessage(const CastMessage& msg) {
    std::vector<uint8_t> buf;
    buf.reserve(64 + msg.payloadUtf8.size());
    writeVarintField(buf,  1, 0);                 // protocol_version = CASTV2_1_0
    writeStringField(buf,  2, msg.sourceId);
    writeStringField(buf,  3, msg.destinationId);
    writeStringField(buf,  4, msg.namespaceUri);
    writeVarintField(buf,  5, 0);                 // payload_type = STRING
    writeStringField(buf,  6, msg.payloadUtf8);
    return buf;
}

// ── Decoder ──────────────────────────────────────────────────────────────────

static bool readVarint(const uint8_t* data, size_t size, size_t& pos, uint64_t& out) {
    out = 0;
    int shift = 0;
    while (pos < size) {
        uint8_t b = data[pos++];
        out |= static_cast<uint64_t>(b & 0x7F) << shift;
        if (!(b & 0x80)) return true;
        shift += 7;
        if (shift >= 64) return false;
    }
    return false;
}

static bool readStringField(const uint8_t* data, size_t size, size_t& pos, std::string& out) {
    uint64_t len;
    if (!readVarint(data, size, pos, len)) return false;
    if (pos + len > size) return false;
    out.assign(reinterpret_cast<const char*>(data + pos), static_cast<size_t>(len));
    pos += static_cast<size_t>(len);
    return true;
}

std::optional<CastMessage> decodeCastMessage(const uint8_t* data, size_t size) {
    CastMessage msg;
    size_t pos = 0;

    while (pos < size) {
        uint64_t tag;
        if (!readVarint(data, size, pos, tag)) return std::nullopt;

        int fieldNum  = static_cast<int>(tag >> 3);
        int wireType  = static_cast<int>(tag & 0x7);

        if (wireType == 0) {
            uint64_t v;
            if (!readVarint(data, size, pos, v)) return std::nullopt;
            // fields 1 (protocol_version) and 5 (payload_type) are intentionally ignored
        } else if (wireType == 2) {
            std::string s;
            if (!readStringField(data, size, pos, s)) return std::nullopt;
            switch (fieldNum) {
                case 2: msg.sourceId      = std::move(s); break;
                case 3: msg.destinationId = std::move(s); break;
                case 4: msg.namespaceUri  = std::move(s); break;
                case 6: msg.payloadUtf8   = std::move(s); break;
                default: break;
            }
        } else {
            return std::nullopt;
        }
    }
    return msg;
}
