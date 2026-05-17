#pragma once
#include "IService.hpp"

class LogService : public IService {
public:
    explicit LogService(int port = 8084);

    std::string_view getName() const override { return "LogService"; }
    void start() override;
    void stop()  override;

private:
    int _port;
};
