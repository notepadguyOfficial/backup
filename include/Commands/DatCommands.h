#ifndef DAT_COMMANDS_H
#define DAT_COMMANDS_H

#include "Command.h"
#include "dat.h"
#include <memory>

class GetCommand : public Command {
private:
    Dat& dat_;
public:
    GetCommand(Dat& dat) : dat_(dat) {}
    std::string Execute(const std::vector<std::string>& args) override;
    std::string GetDescription() override;
};

class SetCommand : public Command {
private:
    Dat& dat_;
public:
    SetCommand(Dat& dat) : dat_(dat) {}
    std::string Execute(const std::vector<std::string>& args) override;
    std::string GetDescription() override;
};

class SaveCommand : public Command {
private:
    Dat& dat_;
public:
    SaveCommand(Dat& dat) : dat_(dat) {}
    std::string Execute(const std::vector<std::string>& args) override;
    std::string GetDescription() override;
};

class LoadCommand : public Command {
private:
    Dat& dat_;
public:
    LoadCommand(Dat& dat) : dat_(dat) {}
    std::string Execute(const std::vector<std::string>& args) override;
    std::string GetDescription() override;
};

#endif