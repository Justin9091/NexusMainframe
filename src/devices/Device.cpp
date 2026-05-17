#include "Devices/Device.hpp"

nlohmann::json Device::toJson() const {
    return {
        {"id",     id},
        {"name",   name},
        {"type",   type},
        {"state",  state},
        {"online", online},
    };
}

Device Device::fromJson(const nlohmann::json& j) {
    Device d;
    d.id     = j.value("id",     "");
    d.name   = j.value("name",   "");
    d.type   = j.value("type",   "");
    d.state  = j.value("state",  nlohmann::json::object());
    d.online = j.value("online", false);
    return d;
}
