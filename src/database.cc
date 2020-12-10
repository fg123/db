#include "database.h"
#include "object.h"
#include "query.h"

#include <fstream>

/** Database Layout:
 *    ~/
 *      DB
 *      collections/
 *          collection-name/
 *              object1hash
 *              object2hash
 *      schema/
 *          objectTypeSchema
 */
Database::Database(fs::path basePath) : basePath(basePath) {
    std::ifstream baseFile(basePath / "DB");
    if (!baseFile) {
        // Could not open DB file, not accessible
        throw "Could not open DB file, invalid DB directory?";
    }
    objectManager.initialize(basePath / "schema", basePath / "collections");
}

COMMAND_FUNCTION_DEFN(List) {
    const std::unordered_map<std::string, std::unordered_set<std::string>>& collections = objectManager.getCollections();
    if (args.size() == 1) {
        for (auto& c : collections) {
            state.output << c.first << std::endl;
        }    
        return;
    }
    const std::string& collectionToFind = args[1];
    if (collections.find(collectionToFind) == collections.end()) {
        state.output << "invalid collection: " << collectionToFind << std::endl;
        return;
    }
    if (collections.find(collectionToFind) == collections.end()) {
        state.output << "invalid collection: " << collectionToFind << std::endl;
        return;
    }
    const auto& collectionContents = collections.at(collectionToFind);
     for (auto& c : collectionContents) {
        state.output << c << std::endl;
    }    
    return;
}

void Database::Query(CommandProcessor& state, const std::string& query) {
    std::cout << "query: '" << query << "'" << std::endl;
    std::istringstream stream(query);
    QueryParser parse(stream, objectManager);

    /*Object* obj = objectManager.getObject(query);
    if (obj) {
        state.output << *obj << std::endl;
    }
    else {
        state.output << "invalid object ID" << std::endl;
    }*/
}