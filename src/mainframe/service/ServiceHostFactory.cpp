//
// Created by jusra on 18-1-2026.
//

#include "mainframe/service/ServiceHostFactory.hpp"

#ifdef _WIN32
#include "mainframe/service/WindowsServiceHost.hpp"
#include "mainframe/service/ForegroundServiceHost.hpp"
#include <windows.h>
#endif

#ifdef _WIN32
    static bool IsRunningAsService() {
        DWORD processId = GetCurrentProcessId();
        DWORD sessionId;
        ProcessIdToSessionId(processId, &sessionId);
        return sessionId == 0; // Session 0 = service
    }
#endif

std::unique_ptr<IServiceHost> ServiceHostFactory::create() {
#ifdef _WIN32
    // Check of we als service draaien
    if (IsRunningAsService()) {
        return std::make_unique<WindowsServiceHost>("NexusService");
    }
#endif
    return std::make_unique<ForegroundServiceHost>();
}
