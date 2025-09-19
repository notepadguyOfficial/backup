#ifndef HELP_COMMAND_H
#define HELP_COMMAND_H

#include "Command.h"
#include "Command.h"  // For CommandParser

class HelpCommand : public Command {
private:
    CommandParser& parser_;
public:
    HelpCommand(CommandParser& parser) : parser_(parser) {}
    std::string Execute(const std::vector<std::string>& args) override;
    std::string GetDescription() override;
};

#endif