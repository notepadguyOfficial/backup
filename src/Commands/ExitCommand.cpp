#include "ExitCommand.h"
#include "Logs.h"

std::string ExitCommand::Execute(const std::vector<std::string> &args)
{
    return "exit";
}

std::string ExitCommand::GetDescription()
{
    return "Exit the application. Usage: exit";
}