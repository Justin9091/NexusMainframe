#include <iostream>
#include <thread>
#include <functional>

#include "NexusMainFrame.hpp"
#include "mainframe/service/ServiceHostFactory.hpp"

// Hulpfunctie: wrap elke thread met try/catch
inline std::thread safe_thread(std::function<void()> func) {
    return std::thread([func]() {
        try {
            func();
        } catch (const std::exception &e) {
            std::cerr << "[Thread Exception] std::exception: " << e.what() << "\n";
        } catch (...) {
            std::cerr << "[Thread Exception] Unknown exception\n";
        }
    });
}

// Hulpfunctie: run een callback veilig (voor service callbacks)
inline void safe_callback(const std::function<void()>& func) {
    try {
        func();
    } catch (const std::exception &e) {
        std::cerr << "[Callback Exception] std::exception: " << e.what() << "\n";
    } catch (...) {
        std::cerr << "[Callback Exception] Unknown exception\n";
    }
}

int main() {
    try {
        auto service = ServiceHostFactory::create();
        NexusMainFrame mainFrame;
        std::thread mainThread;

        service->run(
            [&]() { // start callback
                safe_callback([&]() {
                    mainThread = safe_thread([&]() {
                        mainFrame.run();
                    });
                });
            },
            [&]() { // stop callback
                safe_callback([&]() {
                    mainFrame.stop();
                    if (mainThread.joinable()) {
                        mainThread.join();
                    }
                });
            }
        );
    } catch (const std::exception &e) {
        std::cerr << "[Main Exception] " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "[Main Exception] Unknown exception\n";
        return 2;
    }

    return 0;
}