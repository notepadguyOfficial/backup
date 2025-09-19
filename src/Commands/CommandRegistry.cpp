#include "CommandRegistry.h"
#include "HelpCommand.h"
#include "ExitCommand.h"

std::map<std::string, std::function<std::unique_ptr<Command>(Dat&)>> CommandRegistry::moduleCommands_;

void CommandRegistry::RegisterModuleCommand(const std::string& name, std::function<std::unique_ptr<Command>(Dat&)> factory) {
    moduleCommands_[name] = factory;
}

void CommandRegistry::RegisterAll(CommandParser& parser, Dat& dat) {
    for (auto& pair : moduleCommands_) {
        parser.RegisterCommand(pair.first, pair.second(dat));
    }
    // Special global commands
    parser.RegisterCommand("help", std::make_unique<HelpCommand>(parser));
    parser.RegisterCommand("exit", std::make_unique<ExitCommand>());
}