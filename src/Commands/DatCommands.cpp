#include "DatCommands.h"
#include "Logs.h"
#include "CommandRegistry.h"
#include <sstream>
#include <algorithm>
#include <iostream>
#include <thread>
#include <chrono>

std::string GetCommand::Execute(const std::vector<std::string>& args) {
    if (args.size() != 1) {
        return "Usage: get <key>";
    }
    std::string value = dat_.get(args[0]);
    if (value.empty()) {
        return "Key not found: " + args[0];
    }
    return args[0] + " = " + value;
}

std::string GetCommand::GetDescription() {
    return "Get the value of a setting key. Usage: get <key>";
}

std::string SetCommand::Execute(const std::vector<std::string>& args) {
    if (args.size() != 2) {
        return "Usage: set <key> <value>";
    }
    dat_.set(args[0], args[1]);
    return "Set " + args[0] + " to " + args[1];
}

std::string SetCommand::GetDescription() {
    return "Set the value of a setting key. Usage: set <key> <value>";
}

std::string SaveCommand::Execute(const std::vector<std::string>& args) {
    if (args.size() != 0) {
        return "Usage: save";
    }

    // Simulate progress bar
    const int total = 40;
    for (int i = 0; i <= total; ++i) {
        int percentage = (i * 100) / total;
        std::string bar = "[";
        for (int j = 0; j < total; ++j) {
            if (j < i) {
                bar += "\033[32m█\033[0m";  // Green filled
            } else {
                bar += "\033[90m░\033[0m";  // Gray empty
            }
        }
        bar += "] " + std::to_string(percentage) + "%";
        std::cout << "\r" << bar << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    // Clear the progress bar line
    std::cout << "\r" << std::string(80, ' ') << "\r" << std::flush;

    if (dat_.SaveToFile(dat_.getFilename())) {
        LOG_INFO("Settings saved successfully.");
        return "";
    } else {
        LOG_INFO("Failed to save settings.");
        return "";
    }
}

std::string SaveCommand::GetDescription() {
    return "Save the current settings to file. Usage: save";
}
std::string LoadCommand::Execute(const std::vector<std::string>& args) {
    if (args.size() != 1) {
        return "Usage: load <filename>";
    }
    if (dat_.LoadFromFile(args[0])) {
        return "Settings loaded from " + args[0];
    } else {
        return "Failed to load settings from " + args[0];
    }
}

std::string LoadCommand::GetDescription() {
    return "Load settings from a file. Usage: load <filename>";
}

// Automatic registration
static struct RegisterDatCommands {
    RegisterDatCommands() {
        CommandRegistry::RegisterModuleCommand("get", [](Dat& dat) {
            return std::make_unique<GetCommand>(dat);
        });
        CommandRegistry::RegisterModuleCommand("set", [](Dat& dat) {
            return std::make_unique<SetCommand>(dat);
        });
        CommandRegistry::RegisterModuleCommand("save", [](Dat& dat) {
            return std::make_unique<SaveCommand>(dat);
        });
        CommandRegistry::RegisterModuleCommand("load", [](Dat& dat) {
            return std::make_unique<LoadCommand>(dat);
        });
    }
} reg_dat_commands;

