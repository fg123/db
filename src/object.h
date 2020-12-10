#ifndef OBJECT_H
#define OBJECT_H

#include <sstream>
#include <map>

#include "object-manager.h"
#include "schema.h"
#include "global.h"

class Field {
public:
    virtual ~Field() {}
    virtual std::string toString() = 0;
    static Field* construct(FieldType type, const std::string& raw);
    virtual int Compare(const Field* other) const = 0;
};

class StringField : public Field {
    std::string underlying;
public:
    StringField(const std::string& raw) : underlying(raw) {}
    virtual std::string toString() override { return underlying; }
    virtual int Compare(const Field* other) const override {
        const StringField* field = dynamic_cast<const StringField*>(other);
        if (field) {
            return underlying.compare(field->underlying);
        }
        throw "Bad compare type";
    }
};

class IntegerField : public Field {
    int64_t underlying;
public:
    IntegerField(const std::string& raw) {
        underlying = std::strtoull(raw.c_str(), nullptr, 10);
    }
    virtual std::string toString() override { return std::to_string(underlying); }
    virtual int Compare(const Field* other) const override {
        const IntegerField* field = dynamic_cast<const IntegerField*>(other);
        if (field) {
            return underlying - field->underlying;
        }
        throw "Bad compare type";
    }
};

class DecimalField : public Field {
    int64_t preDecimal;
    uint64_t postDecimal;
public:
    DecimalField(const std::string& raw) {
        std::istringstream s(raw);
        s >> preDecimal;
        if (s.peek() == '.') {
            char decimal;
            s >> decimal;
            s >> postDecimal;
        }
        else {
            postDecimal = 0;
        }
    }
    virtual std::string toString() override {
        return std::to_string(preDecimal) + "." + std::to_string(postDecimal);
    }
    virtual int Compare(const Field* other) const override {
        const DecimalField* field = dynamic_cast<const DecimalField*>(other);
        if (field) {
            if (preDecimal == field->preDecimal) {
                return postDecimal - field->postDecimal;
            }
            else {
                return preDecimal - field->preDecimal;
            }
        }
        throw "Bad compare type";
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
    virtual int Compare(const Field* other) const override {
        const DoubleField* field = dynamic_cast<const DoubleField*>(other);
        if (field) {
            return underlying - field->underlying;
        }
        throw "Bad compare type";
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
};

class ListField : public Field {
    std::vector<Field*> underlying;
public:
    ListField() {}
    ~ListField() { for (auto field : underlying) { delete field; }}

    const std::vector<Field*>& getContents() const { return underlying; }
    void add(Field* field) { underlying.push_back(field); }
    virtual std::string toString() override {
        std::ostringstream output;
        
        for (size_t i = 0 ; i < underlying.size(); i++) {
            if (i != 0) output << ", ";
            output << underlying[i]->toString();
        }

        return output.str();
    }
    virtual int Compare(const Field* other) const override {
       // TODO: Implement this
       return 1;
    }
};

class Object {
    std::map<FieldID, Field*> fields;
    Schema* schema;
    std::string objectId;
    fs::path path;
    ObjectManager& objectManager;

public:
    Object(ObjectManager& manager, const fs::path& path);
    ~Object();
    friend std::ostream& operator<<(std::ostream& os, const Object& obj);
    const Schema* getSchema() { return schema; }
    Field* getField(const FieldID& id) { return fields.at(id); }
};

std::ostream& operator<<(std::ostream& os, const Object& obj);

#endif