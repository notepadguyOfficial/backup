#ifndef EXIT_COMMAND_H
#define EXIT_COMMAND_H

#include "Command.h"

class ExitCommand : public Command {
public:
    ExitCommand() {}
    std::string Execute(const std::vector<std::string>& args) override;
    std::string GetDescription() override;
};

#endif