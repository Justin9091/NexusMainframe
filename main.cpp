#include "NexusMainFrame.hpp"
#include <csignal>
#include <iostream>

NexusMainFrame* g_mainFrame = nullptr;

int main() {
    NexusMainFrame mainFrame;
    g_mainFrame = &mainFrame;

    std::cout << "[main] Starting NexusMainFrame (press Ctrl+C to stop)..." << std::endl;
    mainFrame.run();

    std::cout << "[main] Application exited cleanly" << std::endl;
    g_mainFrame = nullptr;
    return 0;
}