//
// Created by jusra on 18-1-2026.
//

#ifndef NEXUSCORE_WINDOWSSERVICEHOST_HPP
#define NEXUSCORE_WINDOWSSERVICEHOST_HPP
#ifdef _WIN32

#include "IServiceHost.hpp"
#include <windows.h>
#include <string>

/**
 * @brief IServiceHost implementation for Windows Service Control Manager (SCM).
 *
 * Registers a service entry point with the SCM via StartServiceCtrlDispatcher.
 * The SCM controls the process lifecycle; stop commands arrive via
 * ServiceCtrlHandler and trigger the @p onStop callback.
 *
 * @note Only compiled on Windows (@c _WIN32).
 */
class WindowsServiceHost : public IServiceHost {
public:
    /**
     * @param serviceName  Service name as registered in the SCM.
     */
    explicit WindowsServiceHost(std::string serviceName);

    /**
     * @brief Registers with the SCM and blocks until a STOP command is received.
     * @param onStart  Called from ServiceMain after service initialisation.
     * @param onStop   Called when the SCM sends a STOP control code.
     */
    void run(StartCallback onStart, StopCallback onStop) override;

private:
    static void WINAPI ServiceMain(DWORD argc, LPSTR* argv);
    static void WINAPI ServiceCtrlHandler(DWORD ctrl);

    static WindowsServiceHost* instance; ///< Singleton used by static SCM callbacks.

    void start();
    void stop();
    void updateStatus(DWORD state);

    std::string           _serviceName;
    SERVICE_STATUS_HANDLE _statusHandle{};
    SERVICE_STATUS        _status{};
    StartCallback         _onStart;
    StopCallback          _onStop;
};

#endif
#endif //NEXUSCORE_WINDOWSSERVICEHOST_HPP