#pragma once
#include <string>
#include <vector>
#include <cstdint>

struct CastMessage {
    std::string sourceId;
    std::string destinationId;
    std::string namespaceUri;
    std::string payloadUtf8;
};
