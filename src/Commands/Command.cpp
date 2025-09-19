#include "Command.h"
#include <sstream>
#include <algorithm>

void CommandParser::RegisterCommand(const std::string& name, std::unique_ptr<Command> cmd) {
    std::lock_guard<std::mutex> lock(mtx_);
    commands_[name] = std::move(cmd);
}

std::string CommandParser::ParseAndExecute(const std::string& input) {
    std::lock_guard<std::mutex> lock(mtx_);
    std::istringstream iss(input);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    if (tokens.empty()) {
        return "No command entered.";
    }
    std::string cmdName = tokens[0];
    std::transform(cmdName.begin(), cmdName.end(), cmdName.begin(), ::tolower);
    auto it = commands_.find(cmdName);
    if (it == commands_.end()) {
        return "Unknown command: " + cmdName;
    }
    std::vector<std::string> args(tokens.begin() + 1, tokens.end());
    return it->second->Execute(args);
}

std::vector<std::string> CommandParser::GetCommandNames() {
    std::vector<std::string> names;
    for (const auto& pair : commands_) {
        names.push_back(pair.first);
    }
    return names;
}

Command* CommandParser::GetCommand(const std::string& name) {
    auto it = commands_.find(name);
    return it != commands_.end() ? it->second.get() : nullptr;
}