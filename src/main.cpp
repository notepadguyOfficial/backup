#include "Logs.h"
#include "dat.h"
#include <iostream>

int main(int argc, char* argv[]) {
    LOG_INFO("Application started");

    Dat settings("settings.dat");

    LOG_INFO("Settings version: {}", settings.get("version"));
    LOG_INFO("Debug mode: {}", settings.get("debug"));

    return 0;
}
