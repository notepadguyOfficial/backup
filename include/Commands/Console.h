#ifndef CONSOLE_H
#define CONSOLE_H

#include "Command.h"
#include "dat.h"
#include <thread>

class Console {
private:
    CommandParser parser_;
    Dat& settings_;
    void Run();
public:
    Console(Dat& settings);
    void run();
};

#endif