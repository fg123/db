#ifndef COMMANDS_H
#define COMMANDS_H

#include <unordered_map>
#include <functional>

class CommandProcessor;
class Database;
using CommandFunction = std::function<void(Database*, CommandProcessor&, const std::vector<std::string>&)>;

#define COMMAND_FUNCTION_DECL(name) void name(CommandProcessor& state, const std::vector<std::string>& args)
#define COMMAND_FUNCTION_DEFN(name) void Database::name(CommandProcessor& state, const std::vector<std::string>& args)

#define REGISTER_COMMAND(name, function) commands[name] = function;

class CommandProcessor {
    std::unordered_map<std::string, CommandFunction> commands;
public:
    std::ostream& output;

    CommandProcessor();

    void executeCommand(Database& db, std::string command);
};

#endif