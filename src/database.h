#ifndef DATABASE_H
#define DATABASE_H

#include <string>

#include "global.h"
#include "object-manager.h"
#include "commands.h"

class Database {
    fs::path basePath;

    ObjectManager objectManager;

public:
    Database(fs::path path);
    ~Database() {}

    void Query(CommandProcessor& state, const std::string& query);

    // Commands
    COMMAND_FUNCTION_DECL(List);
};
#endif
