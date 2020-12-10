#ifndef QUERY_H
#define QUERY_H

/**
 * Query Parser 
 */
#include <iostream>
#include <vector>

#include "token.h"
#include "object.h"

/** The Query Language is based on ObjectSetExpression, FieldExpression, and FilterExpressions
 *  ObjectSetExpression: COLLECTION | OBJECT_ID | PipelineExpression
 *  PipelineExpression: ObjectSetExpression >> FilterExpression
 *  FilterExpression: FieldExpression == FieldExpression
 *  FieldExpression: 
 *      CONSTANT
 *      IDENTIFIER
 *      IDENTIFIER [ NUMBER ]  // todo
 *      IDENTIFIER . FieldExpression
 */

enum class FilterOperator {
    EQUAL,
    GREATER_THAN,
    LESS_THAN,
    NOT_EQUAL
};

// Resolves to a set of objects
class ObjectSetExpression {
public:
    virtual ~ObjectSetExpression() {}
    static ObjectSetExpression* Parse(const std::vector<Token>& tokens, size_t& i);
    // Cursor / iterator style, returns nullptr when end of the iterator
    virtual Object* Next(ObjectManager& manager) = 0;
};

// Collection reference or not
class LiteralSetExpression : public ObjectSetExpression {
    Token identifier;
    std::vector<ObjectID> objects;
    bool filled = false;
    size_t index = 0;
public:
    LiteralSetExpression(Token identifier) : identifier(identifier) {}
    virtual Object* Next(ObjectManager& manager) {
        if (!filled) {
            // Resolve identifier
            filled = true;
            auto& collections = manager.getCollections();
            if (collections.find(identifier.value) != collections.end()) {
                objects.insert(objects.begin(), collections.at(identifier.value).begin(),
                    collections.at(identifier.value).end());
            }
        }
        if (index >= objects.size()) return nullptr;
        ObjectID first = objects[index];
        index += 1;
        return manager.getObject(first);
    }
};

// Resolves into a Field
class FieldExpression {
public:
    virtual ~FieldExpression() {}
    virtual Field* Resolve(Object* obj) = 0;
    static FieldExpression* Parse(const std::vector<Token>& tokens, size_t& i);
};

class ConstantFieldExpression : public FieldExpression {
    Field* constantValue;
public:
    ConstantFieldExpression(Field* field) : constantValue(field) {}
    ~ConstantFieldExpression() { delete constantValue; }
    virtual Field* Resolve(Object* obj) override {
        return constantValue;
    }
};

class SingleFieldExpression : public FieldExpression {
    Token identifier;
public:
    SingleFieldExpression(Token identifier) : identifier(identifier) {}
    virtual Field* Resolve(Object* obj) override {
        FieldID field = obj->getSchema()->getFieldIDForFieldName(identifier.value);
        return obj->getField(field);
    }
};

// Returns if an object goes through the pipeline
class FilterExpression {
    FieldExpression* left;
    FilterOperator op;
    FieldExpression* right;
public:
    FilterExpression(FieldExpression* left, FilterOperator op, FieldExpression* right) : left(left), op(op), right(right) {}
    virtual ~FilterExpression() { delete left; delete right; }
    virtual bool FilterThrough(Object* obj) { 
        const Field* leftField = left->Resolve(obj);
        const Field* rightField = right->Resolve(obj);
        int cmpResult = leftField->Compare(rightField);
        if (op == FilterOperator::EQUAL) {
            return cmpResult == 0;
        }
        else if (op == FilterOperator::NOT_EQUAL) {
            return cmpResult != 0;
        }
        else if (op == FilterOperator::GREATER_THAN) {
            return cmpResult > 0;
        }
        else if (op == FilterOperator::LESS_THAN) {
            return cmpResult < 0;
        }
    }
    static FilterExpression* Parse(const std::vector<Token>& tokens, size_t& i);
};


class PipelineExpression : public ObjectSetExpression {
    ObjectSetExpression* left;
    FilterExpression* right;
public:
    PipelineExpression(ObjectSetExpression* left, FilterExpression* right) : left(left), right(right) {}
    virtual ~PipelineExpression() { delete left; delete right; }
    virtual Object* Next(ObjectManager& manager) override {
        while (Object* obj = left->Next(manager)) {
            if (right->FilterThrough(obj)) {
                // Pass through the first available
                return obj;
            }
        }
        return nullptr;
    }
};

class Query {
    ObjectSetExpression* expr;
public:
    Query(const std::vector<Token>& tokens);
    ~Query() { delete expr; } 
    Object* Next(ObjectManager& manager) {
        return expr->Next(manager);
    }
};

class QueryParser {
    void handleString(std::istream& input, std::vector<Token>& tokens, char end);
    bool match(std::istream& input, char c);
    bool scanToken(std::istream& input, std::vector<Token>& tokens);
    void scanTokens(std::istream& input, std::vector<Token>& tokens);
public:
    QueryParser(std::istream& input, ObjectManager& manager);
};
#endif