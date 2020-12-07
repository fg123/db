#include "database.h"

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
