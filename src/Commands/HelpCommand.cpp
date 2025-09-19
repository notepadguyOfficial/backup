#include "HelpCommand.h"
#include "Logs.h"
#include <algorithm>

std::string HelpCommand::Execute(const std::vector<std::string>& args) {
    if (args.empty()) {
        // List all commands
        auto names = parser_.GetCommandNames();
        std::string result = "Available commands:\n";
        for (const auto& name : names) {
            result += "  " + name + "\n";
        }
        LOG_INFO("{}", result);
        return "";
    } else {
        // Show description for specific command
        std::string cmdName = args[0];
        std::transform(cmdName.begin(), cmdName.end(), cmdName.begin(), ::tolower);
        Command* cmd = parser_.GetCommand(cmdName);
        if (cmd) {
            LOG_INFO("{}", cmd->GetDescription());
            return "";
        } else {
            return "Unknown command: " + cmdName;
        }
    }
}

std::string HelpCommand::GetDescription() {
    return "Show help for commands. Usage: help [command]";
}