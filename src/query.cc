#include "query.h"

#include <cctype>

QueryParser::QueryParser(std::istream& input, ObjectManager& manager) {
    // Tokenize
    std::vector<Token> tokens;
    scanTokens(input, tokens);
    std::cout << "Output Tokens" << std::endl;
    for (auto& token : tokens) {
        std::cout << token << std::endl;
    }
    Query query{tokens};
    while (Object* obj = query.Next(manager)) {
        std::cout << *obj << std::endl;
    }
}

bool QueryParser::match(std::istream& input, char c) {
    if (input.peek() == c) {
        input >> c;
        return true;
    }
    return false;
}

void QueryParser::handleString(std::istream& input, std::vector<Token>& tokens, char end) {
    std::string strValue;
    while (input.peek() != end && !input.eof()) {
        char c;
        input >> c;
        strValue += c;
        if (c == '\\' && !input.eof()) {
            // Ignore next character, read in too
            input >> c;
            strValue += c;
        }
    }
    
    if (input.eof()) {
        throw "Unterminated string!";
    }
    // end token
    char c;
    input >> c;
    if (c != end) throw "invalid end";
    tokens.emplace_back(Token::Type::STRING, strValue);
}

bool QueryParser::scanToken(std::istream& input, std::vector<Token>& tokens) {
    char c;
    input >> c;
    if (!input) return false;
    switch (c) {
        case '[': 
            tokens.emplace_back(Token::Type::LEFT_BRACK, "[");
            break;
        case ']': 
            tokens.emplace_back(Token::Type::RIGHT_BRACK, "[");
            break;
        case '=': 
            if (match(input, '=')) {
                tokens.emplace_back(Token::Type::EQUAL_EQUAL, "==");
            }
            break;
        case '.':
            tokens.emplace_back(Token::Type::DOT, ".");
            break;
        case '<':
            if (match(input, '<')) {
                tokens.emplace_back(Token::Type::PIPE_LEFT, "<<");
            }
            break;
        case '>':
            if (match(input, '>')) {
                tokens.emplace_back(Token::Type::PIPE_RIGHT, ">>");
            }
            break;
        case ' ':
		case '\r':
		case '\n':
		case '\t':
			break;
        case '"':
            handleString(input, tokens, '"');
            break;
        case '\'':
            handleString(input, tokens, '\'');
            break;
        default:
			if (std::isdigit(c)) {
                std::string strValue;
                strValue += c;
                while (std::isdigit(input.peek())) {
                    input >> c;
                    strValue += c;
                }
                if (input.peek() == '.') {
                    // Decimal
                    input >> c;
                    strValue += c;
                    while (std::isdigit(input.peek())) {
                        input >> c;
                        strValue += c;
                    }
                }
                tokens.emplace_back(Token::Type::NUMBER, strValue);
			}
			else if (std::isalpha(c)) {
                std::string strValue;
                strValue += c;
                while (std::isalnum(input.peek())) {
                    input >> c;
                    strValue += c;
                }
                tokens.emplace_back(Token::Type::IDENTIFIER, strValue);
			}
			else {
				throw "Unexpected character in scanning: " + std::to_string(c);
			}
			break;
    }
    return true;
}

void QueryParser::scanTokens(std::istream& input, std::vector<Token>& tokens) {
    while (scanToken(input, tokens));
}

std::ostream& operator<<(std::ostream& os, const Token& obj) {
    switch(obj.type) {
        case Token::Type::IDENTIFIER: os << "[IDENTIFIER]"; break;
        case Token::Type::STRING: os << "[STRING]"; break;
        case Token::Type::NUMBER: os << "[NUMBER]"; break;
        case Token::Type::PIPE_RIGHT: os << "[PIPE_RIGHT]"; break;
        case Token::Type::PIPE_LEFT: os << "[PIPE_LEFT]"; break;
        case Token::Type::DOT: os << "[DOT]"; break;
        case Token::Type::EQUAL_EQUAL: os << "[EQUAL_EQUAL]"; break;
        case Token::Type::LEFT_BRACK: os << "[LEFT_BRACK]"; break;
        case Token::Type::RIGHT_BRACK: os << "[RIGHT_BRACK]"; break;
    }
    os << ": \"" << obj.value << "\"";
    return os;
}

Token Expect(const std::vector<Token>& tokens, size_t& i, Token::Type type) {
    if (i < tokens.size() && tokens[i].type == type) {
        i++;
        return tokens[i - 1];
    }
    throw "Expected " + std::to_string((int)type) + " token!";
}

bool Match(const std::vector<Token>& tokens, size_t& i, Token::Type type) {
    if (i < tokens.size() && tokens[i].type == type) {
        i++;
        return true;
    }
    return false;
}

Query::Query(const std::vector<Token>& tokens) {
    size_t iter = 0;
    expr = ObjectSetExpression::Parse(tokens, iter);
}

ObjectSetExpression* ObjectSetExpression::Parse(const std::vector<Token>& tokens, size_t& i) {
    Token ident = Expect(tokens, i, Token::Type::IDENTIFIER);
    if (Match(tokens, i, Token::Type::PIPE_RIGHT)) {
        // Pipeline
        FilterExpression* right = FilterExpression::Parse(tokens, i);
        return new PipelineExpression(new LiteralSetExpression(ident), right);
    }
    else {
        return new LiteralSetExpression(ident);
    }
}

FilterExpression* FilterExpression::Parse(const std::vector<Token>& tokens, size_t& i) {
    FieldExpression* left = FieldExpression::Parse(tokens, i);
    FilterOperator op;
    if (Match(tokens, i, Token::Type::EQUAL_EQUAL)) {
        op = FilterOperator::EQUAL;
    }
    else {
        throw "Invalid operator for filter: " + tokens[i].value;
    }
    FieldExpression* right = FieldExpression::Parse(tokens, i);
    return new FilterExpression(left, op, right);
}

FieldExpression* FieldExpression::Parse(const std::vector<Token>& tokens, size_t& i) {
    if (Match(tokens, i, Token::Type::NUMBER)) {
        const Token& token = tokens[i - 1];
        return new ConstantFieldExpression(new DecimalField(token.value));
    }
    else if (Match(tokens, i, Token::Type::STRING)) {
        const Token& token = tokens[i - 1];
        return new ConstantFieldExpression(new StringField(token.value));
    }
    else if (Match(tokens, i, Token::Type::IDENTIFIER)) {
        const Token& token = tokens[i - 1];
        return new SingleFieldExpression(token);
    }
    else {
        // Crash
        throw "Invalid field expression";
    }
}