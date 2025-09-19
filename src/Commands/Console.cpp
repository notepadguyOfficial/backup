#include "Console.h"
#include "CommandRegistry.h"
#include "Logs.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

Console::Console(Dat& settings, WebSocketHandler& wsHandler) : settings_(settings), wsHandler_(wsHandler) {
    CommandRegistry::RegisterAll(parser_, settings_);
}

void Console::Run() {
    std::string input;
    while (true) {
        std::cout << "[CMD] >> " << std::flush;
        std::getline(std::cin, input);
        std::cout << "\033[1A\r" << std::string(50, ' ') << "\r" << std::flush;  // Clear the prompt line
        std::string output = parser_.ParseAndExecute(input);
        if (output == "exit") {
            // Shutdown websocket first
            wsHandler_.stop();
            // Disable auto-save to prevent logging on destruction
            settings_.disableAutoSave();
            // Animated exit countdown with responsive periods
            for (int i = 5; i > 0; --i) {
                for (int dots = 1; dots <= 5; ++dots) {
                    std::cout << "\rExiting in " << i << std::string(dots, '.') << std::string(5 - dots, ' ') << std::flush;
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                }
            }
            std::cout << "\r" << std::flush; // Clear the prompt line
            LOG_INFO("Application exited successfully");
            break;
        }
        if (!output.empty()) {
            LOG_INFO("{}", output);
        }
    }
}

void Console::run() {
    Run();
}