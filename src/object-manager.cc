#include "object-manager.h"
#include "object.h"

ObjectManager::ObjectManager() {

}

ObjectManager::~ObjectManager() {

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
    // Load all objects for now 
    for (auto& dir : fs::directory_iterator(collectionsDir)) {
        if (dir.is_directory()) {
            for (auto& objPath : fs::directory_iterator(dir)) {
                const std::string id = objPath.path().filename().string();
                objects[id] = new Object(*this, objPath.path());
                std::cout << *objects[id] << std::endl;
            }
        }
    }
    std::cout << "Loaded " << objects.size() << " objects." << std::endl;
}