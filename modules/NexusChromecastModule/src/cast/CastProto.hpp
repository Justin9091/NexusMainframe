#pragma once
#include "CastMessage.hpp"
#include <optional>

std::vector<uint8_t>       encodeCastMessage(const CastMessage& msg);
std::optional<CastMessage> decodeCastMessage(const uint8_t* data, size_t size);
