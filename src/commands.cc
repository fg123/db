#include <iostream>
#include <iomanip>
#include <sstream>

#include "commands.h"
#include "database.h"

CommandProcessor::CommandProcessor() : output(std::cout) {
    REGISTER_COMMAND("ls", CommandFunction{&Database::List});
}

void CommandProcessor::executeCommand(Database& db, std::string command) {
    std::istringstream iss(command);
    std::vector<std::string> args;
    std::string s;

    while (iss >> std::quoted(s)) {
        args.push_back(s);
    }

    if (args.size() == 0) {
        return;
    }

    if (commands.find(args[0]) != commands.end()) {
        // Run Command
        commands[args[0]](&db, *this, args);
        return;
    }

    // Command might be a query / reference
    db.Query(*this, command);
}