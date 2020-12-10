#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <iostream>

struct Token {
    enum class Type {
        IDENTIFIER,
        STRING, 
        NUMBER,
        PIPE_RIGHT,     // >>
        PIPE_LEFT,      // <<
        DOT,            // .
        EQUAL_EQUAL,    // ==
        LEFT_BRACK,     // [
        RIGHT_BRACK,    // ]
    };
    Type type;
    std::string value;
    Token(Type type, std::string value) : type(type), value(value) {}
    friend std::ostream& operator<<(std::ostream& os, const Token& obj);
};
std::ostream& operator<<(std::ostream& os, const Token& obj);

#endif