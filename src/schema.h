#ifndef SCHEMA_H
#define SCHEMA_H

#include <string>
#include <filesystem>
#include <unordered_map>

#include "global.h"

using FieldID = uint32_t;

// Supported Data Types
enum class FieldType {
    String,
    Integer,
    Decimal,
    Double,
    Time,
    Reference,
    List
};

struct FieldDefinition {
    std::string name;
    FieldType type;
    virtual ~FieldDefinition() { }
};

struct ReferenceDefinition : FieldDefinition {
    std::string referencedSchemaName;
};

struct ListFieldDefinition : FieldDefinition {
    FieldDefinition* underlyingType;
    ~ListFieldDefinition() { delete underlyingType; }
};

class Schema {
    fs::path path;
    std::unordered_map<FieldID, FieldDefinition*> fields;
    std::unordered_map<std::string, FieldID> fieldNameLookup;
    std::string name;

    void processSchemaV1(std::istream& stream);
public:
    Schema(std::string name, fs::path path);
    ~Schema() {
        for (auto& it : fields) {
            delete it.second;
        }
    }

    const FieldDefinition* getFieldDefinition(FieldID id) { return fields[id]; }
    const FieldID getFieldIDForFieldName(const std::string& name) const { return fieldNameLookup.at(name); }
    std::string getName() const { return name; }
};
#endif
