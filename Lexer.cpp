// Lexer.h
#pragma once
#include <string>
#include <vector>

enum class TokenType {
    Keyword, Identifier, String, Number, Symbol, EndOfFile, Error
};

struct Token {
    TokenType type;
    std::string value;
    int line, col;
};

class Lexer {
    std::string src;
    size_t pos = 0, line = 1, col = 1;
public:
    Lexer(const std::string& s) : src(s) {}
    Token next();
    std::vector<Token> tokenize();
};

// Lexer.cpp
#include "Lexer.h"
#include <cctype>

Token Lexer::next() {
    // Whitespace skipping, simple token extraction (keywords, idents, strings, numbers, symbols)
    // ...[elided for brevity, can expand as needed]
    // Return Token{...}
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    Token t;
    do {
        t = next();
        tokens.push_back(t);
    } while (t.type != TokenType::EndOfFile);
    return tokens;
}
