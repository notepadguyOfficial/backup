#ifndef COMMAND_REGISTRY_H
#define COMMAND_REGISTRY_H

#include <string>
#include <functional>
#include <memory>
#include <map>
#include "Command.h"
#include "dat.h"

class CommandRegistry {
public:
    static void RegisterModuleCommand(const std::string& name, std::function<std::unique_ptr<Command>(Dat&)> factory);
    static void RegisterAll(CommandParser& parser, Dat& dat);
private:
    static std::map<std::string, std::function<std::unique_ptr<Command>(Dat&)>> moduleCommands_;
};

#endif