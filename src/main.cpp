#include "Logs.h"
#include "dat.h"
#include "Console.h"
#include "WebSocketHandler.h"
#include <thread>

int main(int argc, char* argv[]) {
    LOG_INFO("Application started");

    Dat settings("settings.dat");

    LOG_INFO("Settings version: {}", settings.get("version"));
    LOG_INFO("Debug mode: {}", settings.get("debug"));

    WebSocketHandler wsHandler(settings);
    std::thread wsThread(&WebSocketHandler::run, &wsHandler);

    Console console(settings, wsHandler);
    std::thread consoleThread(&Console::run, &console);

    consoleThread.join();
    wsThread.join();

    return 0;
}
