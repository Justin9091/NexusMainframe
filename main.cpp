#include "NexusMainFrame.hpp"
#include <windows.h>
#include <iostream>

NexusMainFrame* g_mainFrame = nullptr;

BOOL WINAPI consoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT || signal == CTRL_CLOSE_EVENT) {
        std::cout << "\nCtrl+C of venster sluiten ontvangen, stoppen...\n";
        if (g_mainFrame) {
            g_mainFrame->stop();
        }
        return TRUE; // signaal afgehandeld
    }
    return FALSE;
}

int main() {
    NexusMainFrame mainFrame;
    g_mainFrame = &mainFrame;

    // registreer console handler
    SetConsoleCtrlHandler(consoleHandler, TRUE);

    mainFrame.run();

    return 0;
}
