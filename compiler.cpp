#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

// === Token Types ===
enum class TokenType {
    KEYWORD, IDENTIFIER, NUMBER, STRING, SYMBOL, DG, END_OF_FILE
};

// === Token Structure ===
struct Token {
    TokenType type;
    std::string value;
    int line;
};

// === Lexer ===
class Lexer {
public:
    explicit Lexer(const std::string& src);
    std::vector<Token> tokenize();

private:
    std::string source;
    size_t pos = 0;
    int line = 1;

    char peek();
    char advance();
    void skipWhitespace();
    Token nextToken();
};

// === AST Nodes ===
struct ASTNode {
    virtual ~ASTNode() = default;
};

struct ProgramNode : ASTNode {
    std::vector<ASTNode*> statements;
};

struct SayNode : ASTNode {
    std::string message;
};

// Add more nodes: ValNode, VarNode, LoopNode, DeriveNode, etc.

// === Parser ===
class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    ProgramNode* parse();

private:
    std::vector<Token> tokens;
    size_t current = 0;

    Token peek();
    Token advance();
    bool match(const std::string& value);
    ASTNode* parseStatement();
};

// === Code Generator ===
class CodeGenerator {
public:
    void generate(ProgramNode* program);
};

// === Main Compiler Driver ===
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: quarterc <source.qtr>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    Parser parser(tokens);
    ProgramNode* program = parser.parse();

    CodeGenerator generator;
    generator.generate(program);

    std::cout << "Compilation successful.\n";
    return 0;
}

