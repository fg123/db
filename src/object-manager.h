#ifndef OBJECT_MANAGER_H
#define OBJECT_MANAGER_H

#include <unordered_map>
#include <unordered_set>

#include "global.h"
#include "schema.h"

class Object;

using ObjectID = std::string;
/** ObjectManager Manages Lifetime of Objects in Memory */
class ObjectManager {
    std::unordered_map<std::string, Schema*> schemaTable;
    std::unordered_map<ObjectID, Object*> objects;
    
    std::unordered_map<std::string, std::unordered_set<ObjectID>> collections;
    std::unordered_map<ObjectID, fs::path> objectPathLookup;

public:
    ObjectManager();
    ~ObjectManager();

    void initialize(fs::path schemaDir, fs::path collectionsDir);

    Schema* getSchema(const std::string& name) { return schemaTable[name]; }

    const std::unordered_map<std::string, std::unordered_set<ObjectID>>& getCollections() { return collections; }
    
    Object* getObject(const ObjectID& id);
};

#endif