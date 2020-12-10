#include "schema.h"
#include <fstream>
#include <sstream>
#include <regex>

FieldDefinition* FieldDefinitionFromString(std::string name, std::string typeStr) {
    // Extract potential template type
    trim(typeStr);
    if (typeStr.rfind("List", 0) == 0) {
        static const std::regex regex(R"rgx(.*\<.*\>)rgx");
        std::smatch templateMatch;
        if (!std::regex_match(typeStr, templateMatch, regex)) {
            throw "List parameter with no type in schema!";
        }
        ListFieldDefinition* def = new ListFieldDefinition;
        def->type = FieldType::List;
        def->name = name;
        def->underlyingType = FieldDefinitionFromString("internalType", templateMatch[1]);
        return def;
    }

    FieldType type;
    if (typeStr == "String") {
        type = FieldType::String;
    }
    else if (typeStr == "Integer") {
        type = FieldType::Integer;
    }
    else if (typeStr == "Decimal") {
        type = FieldType::Decimal;
    }
    else if (typeStr == "Double") {
        type = FieldType::Double;
    }
    else if (typeStr == "Time") {
        type = FieldType::Time;
    }
    else {
        // Assume it's a reference to something
        type = FieldType::Reference;
        ReferenceDefinition* def = new ReferenceDefinition;
        def->type = FieldType::Reference;
        def->name = name;
        def->referencedSchemaName = typeStr;
        return def;
    }
    
    FieldDefinition* def = new FieldDefinition;
    def->name = name;
    def->type = type;
    return def;
}


Schema::Schema(std::string name, fs::path path) : name(name) {
    std::ifstream schemaFile(path);
    std::string version;
    schemaFile >> version;

    if (version == "SCHEMA-1") {
        processSchemaV1(schemaFile);
    }
}

void Schema::processSchemaV1(std::istream& stream) {
    std::string line;// \s*=\s*(.*)\s*:\s*(.*)
    static const std::regex regex(R"rgx(^([0-9]+)\s*=\s*(.*)\s*:\s*(.*)$)rgx");
    while (std::getline(stream, line)) {
        // Each line is in the form:
        // 1 = dataRaw
        trim(line);
        if (line.size() > 0) {
            std::smatch parts_match;
            if (std::regex_match(line, parts_match, regex)) {
                const FieldID id = std::atoi(parts_match[1].str().c_str());
                const std::string fieldName = parts_match[2].str();
                const std::string fieldTypeStr = parts_match[3].str();
                fields[id] = FieldDefinitionFromString(fieldName, fieldTypeStr);
                fieldNameLookup[fieldName] = id;
            }
        }
    }
}