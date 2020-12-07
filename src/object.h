#ifndef OBJECT_H
#define OBJECT_H

#include <sstream>

#include "object-manager.h"
#include "schema.h"
#include "global.h"

class Field {
public:
    virtual ~Field() {}
    virtual std::string toString() = 0;
    static Field* construct(FieldType type, const std::string& raw);
};

class StringField : public Field {
    std::string underlying;
public:
    StringField(const std::string& raw) : underlying(raw) {}
    virtual std::string toString() override { return underlying; }
};

class IntegerField : public Field {
    int64_t underlying;
public:
    IntegerField(const std::string& raw) {
        underlying = std::strtoull(raw.c_str(), nullptr, 10);
    }
    virtual std::string toString() override { return std::to_string(underlying); }
};

class DecimalField : public Field {
    int64_t preDecimal;
    uint64_t postDecimal;
public:
    DecimalField(const std::string& raw) {
        std::istringstream s(raw);
        s >> preDecimal;
        char decimal;
        s >> decimal;
        if (decimal != '.') throw "Invalid Decimal Format";
        s >> postDecimal;
    }
    virtual std::string toString() override {
        return std::to_string(preDecimal) + "." + std::to_string(postDecimal);
    }
};

class DoubleField : public Field {
    double underlying;
public:
    DoubleField(const std::string& raw) {
        underlying = std::atof(raw.c_str());
    }
    virtual std::string toString() override {
        return std::to_string(underlying);
    }
};

// Stores a integer value of the timestamp
class TimeField : public IntegerField {
public:
    TimeField(const std::string& raw) : IntegerField(raw) {}
};

// Stores a string ID of the pointed-to object
class ReferenceField : public StringField {
public:
    ReferenceField(const std::string& raw) : StringField(raw) {}
    virtual std::string toString() override { 
        return "R:" + StringField::toString();
    }
};

class ListField : public Field {
    std::vector<Field*> underlying;
public:
    ListField() {}
    ~ListField() { for (auto field : underlying) { delete field; }}

    void add(Field* field) { underlying.push_back(field); }
    virtual std::string toString() override {
        std::ostringstream output;
        
        for (size_t i = 0 ; i < underlying.size(); i++) {
            if (i != 0) output << ", ";
            output << underlying[i]->toString();
        }

        return output.str();
    }
};

class Object {
    std::unordered_map<FieldID, Field*> fields;
    Schema* schema;
    std::string objectId;

public:
    Object(ObjectManager& manager, const fs::path& path);
    ~Object() {}
    friend std::ostream& operator<<(std::ostream& os, const Object& obj);
};

std::ostream& operator<<(std::ostream& os, const Object& obj);

#endif