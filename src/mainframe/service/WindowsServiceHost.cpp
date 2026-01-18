#ifdef _WIN32

#include "mainframe/service/WindowsServiceHost.hpp"
#include <fstream>
#include <ctime>

WindowsServiceHost* WindowsServiceHost::instance = nullptr;

// Helper functie voor logging
static void Log(const std::string& message) {
    std::ofstream log("C:\\Temp\\nexus_service.log", std::ios::app);
    auto now = std::time(nullptr);
    log << "[" << now << "] " << message << std::endl;
    log.close();
}

WindowsServiceHost::WindowsServiceHost(std::string serviceName)
    : _serviceName(std::move(serviceName)) {
    Log("WindowsServiceHost constructor called with name: " + _serviceName);
}

void WindowsServiceHost::run(StartCallback onStart, StopCallback onStop) {
    Log("WindowsServiceHost::run() called");
    _onStart = std::move(onStart);
    _onStop  = std::move(onStop);

    instance = this;

    SERVICE_TABLE_ENTRY serviceTable[] = {
        { const_cast<LPSTR>(_serviceName.c_str()), ServiceMain },
        { nullptr, nullptr }
    };

    Log("Calling StartServiceCtrlDispatcher...");
    if (!StartServiceCtrlDispatcher(serviceTable)) {
        DWORD error = GetLastError();
        Log("StartServiceCtrlDispatcher FAILED with error: " + std::to_string(error));
    } else {
        Log("StartServiceCtrlDispatcher completed");
    }
}

void WINAPI WindowsServiceHost::ServiceMain(DWORD, LPSTR*) {
    Log("ServiceMain() called");

    instance->_statusHandle =
        RegisterServiceCtrlHandler(instance->_serviceName.c_str(), ServiceCtrlHandler);

    if (instance->_statusHandle == nullptr) {
        Log("RegisterServiceCtrlHandler FAILED");
        return;
    }
    Log("RegisterServiceCtrlHandler succeeded");

    instance->_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    instance->_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    instance->updateStatus(SERVICE_START_PENDING);
    Log("Status set to START_PENDING");

    instance->start();
    Log("start() completed");

    instance->_status.dwControlsAccepted |= SERVICE_ACCEPT_SHUTDOWN;
    instance->updateStatus(SERVICE_RUNNING);
    Log("Status set to RUNNING");
}

void WINAPI WindowsServiceHost::ServiceCtrlHandler(DWORD ctrl) {
    Log("ServiceCtrlHandler called with ctrl=" + std::to_string(ctrl));
    if (ctrl == SERVICE_CONTROL_STOP || ctrl == SERVICE_CONTROL_SHUTDOWN) {
        instance->updateStatus(SERVICE_STOP_PENDING);
        instance->stop();
        instance->updateStatus(SERVICE_STOPPED);
        Log("Service stopped");
    }
}

void WindowsServiceHost::start() {
    Log("WindowsServiceHost::start() called");
    if (_onStart) {
        try {
            Log("Calling _onStart callback...");
            _onStart();
            Log("_onStart callback completed successfully");
        } catch (const std::exception& e) {
            Log(std::string("EXCEPTION in _onStart: ") + e.what());
        } catch (...) {
            Log("UNKNOWN EXCEPTION in _onStart");
        }
    } else {
        Log("WARNING: _onStart callback is nullptr");
    }
}

void WindowsServiceHost::stop() {
    Log("WindowsServiceHost::stop() called");
    if (_onStop) {
        try {
            Log("Calling _onStop callback...");
            _onStop();
            Log("_onStop callback completed successfully");
        } catch (const std::exception& e) {
            Log(std::string("EXCEPTION in _onStop: ") + e.what());
        } catch (...) {
            Log("UNKNOWN EXCEPTION in _onStop");
        }
    }
}

void WindowsServiceHost::updateStatus(DWORD state) {
    _status.dwCurrentState = state;
    _status.dwWin32ExitCode = NO_ERROR;
    _status.dwWaitHint = 0;
    SetServiceStatus(_statusHandle, &_status);
    Log("Status updated to: " + std::to_string(state));
}

#endif