#ifndef CONSOLE_H
#define CONSOLE_H

#include "Command.h"
#include "dat.h"
#include "WebSocketHandler.h"
#include <thread>

class Console {
private:
    CommandParser parser_;
    Dat& settings_;
    WebSocketHandler& wsHandler_;
    void Run();
public:
    Console(Dat& settings, WebSocketHandler& wsHandler);
    void run();
    static void ReDrawPrompt();
};

#endif