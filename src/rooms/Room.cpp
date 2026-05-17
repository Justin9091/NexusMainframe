#include "Rooms/Room.hpp"

nlohmann::json Room::toJson() const {
    return {
        {"id",        id},
        {"name",      name},
        {"deviceIds", deviceIds},
    };
}

Room Room::fromJson(const nlohmann::json& j) {
    Room r;
    r.id        = j.value("id",   "");
    r.name      = j.value("name", "");
    r.deviceIds = j.value("deviceIds", std::vector<std::string>{});
    return r;
}
