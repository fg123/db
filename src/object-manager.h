#ifndef OBJECT_MANAGER_H
#define OBJECT_MANAGER_H

#include <unordered_map>

#include "global.h"
#include "schema.h"

class Object;

/** ObjectManager Manages Lifetime of Objects in Memory */
class ObjectManager {
    std::unordered_map<std::string, Schema*> schemaTable;
    std::unordered_map<std::string, Object*> objects;

public:
    ObjectManager();
    ~ObjectManager();

    void initialize(fs::path schemaDir, fs::path collectionsDir);

    Schema* getSchema(std::string name) { return schemaTable[name]; }
};

#endif