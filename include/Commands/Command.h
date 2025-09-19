#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <mutex>

class Command {
public:
    virtual ~Command() = default;
    virtual std::string Execute(const std::vector<std::string>& args) = 0;
    virtual std::string GetDescription() = 0;
};

class CommandParser {
private:
    std::map<std::string, std::unique_ptr<Command>> commands_;
    std::mutex mtx_;
public:
    void RegisterCommand(const std::string& name, std::unique_ptr<Command> cmd);
    std::string ParseAndExecute(const std::string& input);
    std::vector<std::string> GetCommandNames();
    Command* GetCommand(const std::string& name);
};

#endif