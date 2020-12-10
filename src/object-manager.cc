#include "object-manager.h"
#include "object.h"

ObjectManager::ObjectManager() {

}

ObjectManager::~ObjectManager() {
    for (auto& object : objects) {
        delete object.second;
    }
}

void ObjectManager::initialize(fs::path schemaDir, fs::path collectionsDir) {
    // Load all the schemas
    for (auto& file : fs::directory_iterator(schemaDir)) {
        if (file.is_regular_file()) {
            const std::string schemaName = file.path().filename().string();
            if (schemaTable.find(schemaName) != schemaTable.end()) {
                throw "Schema already exists";
            }
            Schema* schema = new Schema(schemaName, file.path());
            schemaTable[schemaName] = schema;
        }
    }
    // Find all objects
    for (auto& dir : fs::directory_iterator(collectionsDir)) {
        if (dir.is_directory()) {
            collections[dir.path().filename().string()];
            for (auto& objPath : fs::directory_iterator(dir)) {
                const ObjectID id = objPath.path().filename().string();
                // Ignore Temporary files created from write
                if (id[id.size() - 1] != '~') {
                    objectPathLookup[id] = objPath;
                    collections[dir.path().filename().string()].insert(id);
                    /* objects[id] = new Object(*this, objPath.path());
                    std::cout << *objects[id] << std::endl; */
                }
            }
        }
    }
    std::cout << "Found " << objectPathLookup.size() << " objects." << std::endl;
}

Object* ObjectManager::getObject(const ObjectID& id) {
    if (objects.find(id) == objects.end()) {
        // Load in object from disk
        if (objectPathLookup.find(id) == objectPathLookup.end()) {
            // Object ID doesn't exist to our knowledge...
            return nullptr;
        }
        objects[id] = new Object(*this, objectPathLookup[id]);
    }
    return objects[id];
}