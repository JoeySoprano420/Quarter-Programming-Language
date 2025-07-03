// AST.h
#pragma once
#include <string>
#include <vector>
#include <memory>

struct Stmt { virtual ~Stmt() = default; };
struct Expr { virtual ~Expr() = default; };

struct SayStmt : Stmt { std::string text; };
struct RetStmt : Stmt {};
struct FuncDecl : Stmt {
    std::string name;
    std::vector<std::unique_ptr<Stmt>> body;
};

// Parser.h
#pragma once
#include "Lexer.h"
#include "AST.h"
#include <vector>
#include <memory>

class Parser {
    std::vector<Token> tokens;
    size_t pos = 0;
public:
    Parser(const std::vector<Token>& toks) : tokens(toks) {}
    std::unique_ptr<FuncDecl> parse();
};

// Parser.cpp (simplified main flow)
#include "Parser.h"
std::unique_ptr<FuncDecl> Parser::parse() {
    // Eat tokens for: quarter func main begin ... end
    // Recognize say "text", ret, etc.
    // Build AST nodes
    // Return AST
}
