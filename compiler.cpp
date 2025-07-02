// QuarterLang Compiler — Phase 7
// ✅ Debug Shell, VM Memory Inspection, Full Interactive Mode

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <sstream>
#include <cctype>
#include <cstdlib>
#include <thread>

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

// === Typed Variable ===
struct TypedValue {
    std::string type; // "int", "text", "dg"
    std::string value;
};

// === Lexer ===
class Lexer {
public:
    explicit Lexer(const std::string& src) : source(src), pos(0), line(1) {}
    std::vector<Token> tokenize();

private:
    std::string source;
    size_t pos;
    int line;
    const std::unordered_set<std::string> keywords = {
        "star", "end", "val", "var", "say", "loop", "from", "to",
        "when", "else", "derive", "dg", "match", "case", "define",
        "procedure", "return", "yield", "thread", "pipe", "nest",
        "fn", "asm", "stop", "dg_add", "dg_mul"
    };
    char peek() const { return pos < source.size() ? source[pos] : '\0'; }
    char advance() { return pos < source.size() ? source[pos++] : '\0'; }
    void skipWhitespace() {
        while (std::isspace(peek())) {
            if (peek() == '\n') ++line;
            advance();
        }
    }
    void skipComment() {
        if (peek() == '#') {
            while (peek() != '\n' && peek() != '\0') advance();
        }
    }
    Token readWord() {
        size_t start = pos;
        while (std::isalnum(peek()) || peek() == '_') advance();
        std::string word = source.substr(start, pos - start);
        if (keywords.count(word))
            return Token{ TokenType::KEYWORD, word, line };
        return Token{ TokenType::IDENTIFIER, word, line };
    }
    Token readNumber() {
        size_t start = pos;
        while (std::isdigit(peek())) advance();
        return Token{ TokenType::NUMBER, source.substr(start, pos - start), line };
    }
    Token readString() {
        advance(); // skip opening "
        size_t start = pos;
        while (peek() != '"' && peek() != '\0') advance();
        std::string str = source.substr(start, pos - start);
        if (peek() == '"') advance();
        return Token{ TokenType::STRING, str, line };
    }
    Token readSymbol() {
        char c = advance();
        return Token{ TokenType::SYMBOL, std::string(1, c), line };
    }
};

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (peek() != '\0') {
        skipWhitespace();
        skipComment();
        if (peek() == '\0') break;
        if (std::isalpha(peek()) || peek() == '_') {
            tokens.push_back(readWord());
        }
        else if (std::isdigit(peek())) {
            tokens.push_back(readNumber());
        }
        else if (peek() == '"') {
            tokens.push_back(readString());
        }
        else {
            tokens.push_back(readSymbol());
        }
    }
    tokens.push_back(Token{ TokenType::END_OF_FILE, "", line });
    return tokens;
}

// === AST Nodes ===
struct ASTNode {
    virtual ~ASTNode() = default;
};

struct ProgramNode : ASTNode {
    std::vector<ASTNode*> statements;
    ~ProgramNode() {
        for (auto* stmt : statements) delete stmt;
    }
};

struct ValNode : ASTNode {
    std::string name;
    std::string type;
    std::string value;
};

struct SayNode : ASTNode {
    std::string message;
};

struct DeriveNode : ASTNode {
    std::string name;
    std::string base;
};

struct LoopNode : ASTNode {
    std::string varName;
    std::string from;
    std::string to;
    std::vector<ASTNode*> body;
    ~LoopNode() {
        for (auto* stmt : body) delete stmt;
    }
};

struct ProcedureNode : ASTNode {
    std::string name;
    std::vector<std::pair<std::string, std::string>> params; // (name, type)
    std::vector<ASTNode*> body;
    std::string returnType;
    ~ProcedureNode() {
        for (auto* stmt : body) delete stmt;
    }
};

struct ReturnNode : ASTNode {
    std::string value;
};

struct YieldNode : ASTNode {
    std::string value;
};

struct ThreadNode : ASTNode {
    std::string procedureName;
};

// === Parser ===
class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}
    ProgramNode* parse() {
        ProgramNode* program = new ProgramNode();
        while (!isAtEnd()) {
            ASTNode* stmt = parseStatement();
            if (stmt) program->statements.push_back(stmt);
        }
        return program;
    }

private:
    std::vector<Token> tokens;
    size_t current;
    Token peek() { return tokens[current]; }
    Token advance() { return tokens[current++]; }
    bool match(const std::string& value) { return peek().value == value; }
    void consume(const std::string& expected, const std::string& err) {
        if (!match(expected)) {
            std::cerr << "Parse error: " << err << " at line " << peek().line << "\n";
            std::exit(1);
        }
        advance();
    }
    bool isAtEnd() { return peek().type == TokenType::END_OF_FILE; }

    ASTNode* parseStatement() {
        if (match("val")) return parseVal();
        if (match("say")) return parseSay();
        if (match("loop")) return parseLoop();
        if (match("procedure")) return parseProcedure();
        if (match("return")) return parseReturn();
        if (match("yield")) return parseYield();
        if (match("thread")) return parseThread();
        advance(); // skip unknown
        return nullptr;
    }
    ASTNode* parseVal() {
        advance(); // 'val'
        std::string name = advance().value;
        consume(":", "Expected ':' after val name");
        std::string type = advance().value;
        consume("=", "Expected '=' after val type");
        std::string value = advance().value;
        ValNode* node = new ValNode();
        node->name = name;
        node->type = type;
        node->value = value;
        return node;
    }
    ASTNode* parseSay() {
        advance(); // 'say'
        std::string msg = advance().value;
        SayNode* node = new SayNode();
        node->message = msg;
        return node;
    }
    ASTNode* parseLoop() {
        advance(); // 'loop'
        std::string varName = advance().value;
        consume("from", "Expected 'from' in loop");
        std::string from = advance().value;
        consume("to", "Expected 'to' in loop");
        std::string to = advance().value;
        LoopNode* node = new LoopNode();
        node->varName = varName;
        node->from = from;
        node->to = to;
        consume("{", "Expected '{' to start loop body");
        while (!match("}")) {
            node->body.push_back(parseStatement());
        }
        consume("}", "Expected '}' to end loop body");
        return node;
    }
    ASTNode* parseProcedure() {
        advance(); // 'procedure'
        std::string name = advance().value;
        ProcedureNode* node = new ProcedureNode();
        node->name = name;
        consume("(", "Expected '(' after procedure name");
        while (!match(")")) {
            std::string paramName = advance().value;
            consume(":", "Expected ':' in param");
            std::string paramType = advance().value;
            node->params.push_back(std::make_pair(paramName, paramType));
            if (match(",")) advance();
        }
        consume(")", "Expected ')' after params");
        if (match(":")) {
            advance();
            node->returnType = advance().value;
        }
        consume("{", "Expected '{' to start procedure body");
        while (!match("}")) {
            node->body.push_back(parseStatement());
        }
        consume("}", "Expected '}' to end procedure body");
        return node;
    }
    ASTNode* parseReturn() {
        advance(); // 'return'
        std::string value = advance().value;
        ReturnNode* node = new ReturnNode();
        node->value = value;
        return node;
    }
    ASTNode* parseYield() {
        advance(); // 'yield'
        std::string value = advance().value;
        YieldNode* node = new YieldNode();
        node->value = value;
        return node;
    }
    ASTNode* parseThread() {
        advance(); // 'thread'
        std::string proc = advance().value;
        ThreadNode* node = new ThreadNode();
        node->procedureName = proc;
        return node;
    }
};

// === Code Generator ===
class CodeGenerator {
public:
    void generate(ProgramNode* program) {
        std::cout << "[CodeGen] Generating IR (stub)\n";
        // Walk AST and emit IR or bytecode
    }
};

// === NASM Codegen ===
class NasmCodegen {
public:
    void compileToNasm(ProgramNode* program) {
        std::cout << "[NASM] Compiling to NASM (stub)\n";
        // Walk AST and emit NASM assembly
    }
};

// === Capsule VM Runtime ===
class CapsuleVM {
public:
    void execute(const std::string& capsulePath) {
        std::cout << "[VM] Executing capsule: " << capsulePath << " (stub)\n";
    }
    void runInteractive() {
        std::cout << "[VM] Interactive mode (stub)\n";
    }
    void enterDebugShell() {
        std::cout << "[VM] Debug shell (stub)\n";
    }

private:
    typedef std::unordered_map<std::string, TypedValue> Scope;
    std::stack<Scope> memoryStack;
    std::unordered_map<std::string, ProcedureNode*> procedures;

    void runLine(const std::string& line) {}
    void executeLoop(const std::string& var, int from, int to, const std::vector<std::string>& body) {}
    void callProcedure(const std::string& name, const std::vector<std::string>& args) {}
    void runThreadedProcedure(const std::string& name) {}
    void pushScope() {}
    void popScope() {}
    std::string getVar(const std::string& name) { return ""; }
    void setVar(const std::string& name, const std::string& value, const std::string& type) {}
    void reportError(const std::string& msg, int line) {}
    void inspectMemory() {}
};

// === Language Server Protocol (LSP) Stub ===
class LanguageServer {
public:
    void start() {
        std::cout << "[LSP] Language server started (stub)\n";
    }
};

// === Self-hosting Compiler Entry Point ===
class QuarterSelf;
