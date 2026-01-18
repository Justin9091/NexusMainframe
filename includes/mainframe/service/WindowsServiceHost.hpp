//
// Created by jusra on 18-1-2026.
//

#ifndef NEXUSCORE_WINDOWSSERVICEHOST_HPP
#define NEXUSCORE_WINDOWSSERVICEHOST_HPP
#ifdef _WIN32

#include "IServiceHost.hpp"
#include <windows.h>
#include <string>

class WindowsServiceHost : public IServiceHost {
public:
    explicit WindowsServiceHost(std::string serviceName);

    void run(StartCallback onStart, StopCallback onStop) override;

private:
    static void WINAPI ServiceMain(DWORD argc, LPSTR* argv);
    static void WINAPI ServiceCtrlHandler(DWORD ctrl);

    static WindowsServiceHost* instance;

    void start();
    void stop();
    void updateStatus(DWORD state);

    std::string _serviceName;
    SERVICE_STATUS_HANDLE _statusHandle{};
    SERVICE_STATUS _status{};
    StartCallback _onStart;
    StopCallback _onStop;
};

#endif
#endif //NEXUSCORE_WINDOWSSERVICEHOST_HPP