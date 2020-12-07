#include "object.h"

#include <fstream>

Field* Field::construct(FieldType type, const std::string& raw) {
    switch (type) {
        case FieldType::String:
            return new StringField(raw);
        case FieldType::Integer:
            return new IntegerField(raw);
        case FieldType::Decimal:
            return new DecimalField(raw);
        case FieldType::Double:
            return new DoubleField(raw);
        case FieldType::Time:
            return new TimeField(raw);
        case FieldType::Reference:
            return new ReferenceField(raw);
        case FieldType::List:
            return new ListField();
    }
}

Object::Object(ObjectManager& objectManager, const fs::path& path) :
        objectId(path.filename().string()) {
    std::ifstream s(path);
    std::string schemaWord, schemaName;
    s >> schemaWord >> schemaName;
    if (schemaWord != "schema") throw "Invalid schema declaration!";
    schema = objectManager.getSchema(schemaName);
    std::string line;
    while (std::getline(s, line)) {
        // Each line is in the form:
        // 1 = dataRaw
        trim(line);
        if (line.size() > 0) {
            FieldID id;
            std::istringstream stream(line);
            stream >> id;
            char equal;
            stream >> equal;
            std::string raw;
            stream >> raw;
            if (schema->getFieldDefinition(id)->type == FieldType::List) {
                if (fields.find(id) == fields.end()) {
                    fields[id] = new ListField();
                }
                ListField* field = dynamic_cast<ListField*>(fields[id]);
                // Get list schema
                const FieldDefinition* def = schema->getFieldDefinition(id);
                const ListFieldDefinition* listDef = dynamic_cast<const ListFieldDefinition*>(def);
                field->add(
                    Field::construct(
                        listDef->underlyingType->type, raw));
            }
            else {
                fields[id] = Field::construct(schema->getFieldDefinition(id)->type, raw);
            }
        }
    }
}

std::ostream& operator<<(std::ostream& os, const Object& obj) {
    os << "[" << obj.objectId << ": " << obj.schema->getName() << "]" << std::endl;
    for (auto& it: obj.fields) {
        // Do stuff
        os << obj.schema->getFieldDefinition(it.first)->name << " = " << it.second->toString() << std::endl; 
    }
    return os;
}