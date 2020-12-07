#ifndef DATABASE_H
#define DATABASE_H

#include <string>

#include "global.h"
#include "object-manager.h"

class Database {
    fs::path basePath;

    ObjectManager objectManager;

public:
    Database(fs::path path);
    ~Database() {}
    
};
#endif
