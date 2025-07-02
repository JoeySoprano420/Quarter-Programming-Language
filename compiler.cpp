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
    explicit Lexer(const std::string& src);
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
    char peek() const;
    char advance();
    void skipWhitespace();
    void skipComment();
    Token readWord();
    Token readNumber();
    Token readString();
    Token readSymbol();
};

// === AST Nodes ===
struct ASTNode {
    virtual ~ASTNode() = default;
};

struct ProgramNode : ASTNode {
    std::vector<ASTNode*> statements;
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
};

struct ProcedureNode : ASTNode {
    std::string name;
    std::vector<std::pair<std::string, std::string>> params; // (name, type)
    std::vector<ASTNode*> body;
    std::string returnType;
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
    explicit Parser(const std::vector<Token>& tokens);
    ProgramNode* parse();

private:
    std::vector<Token> tokens;
    size_t current;
    Token peek();
    Token advance();
    bool match(const std::string& value);
    void consume(const std::string& expected, const std::string& err);
    bool isAtEnd();
    ASTNode* parseStatement();
    ASTNode* parseVal();
    ASTNode* parseSay();
    ASTNode* parseDerive();
    ASTNode* parseLoop();
    ASTNode* parseProcedure();
    ASTNode* parseReturn();
    ASTNode* parseYield();
    ASTNode* parseThread();
};

// === Code Generator ===
class CodeGenerator {
public:
    void generate(ProgramNode* program);
};

// === NASM Codegen ===
class NasmCodegen {
public:
    void compileToNasm(ProgramNode* program);
};

// === Capsule VM Runtime ===
class CapsuleVM {
public:
    void execute(const std::string& capsulePath);
    void runInteractive();
    void enterDebugShell();

private:
    using Scope = std::unordered_map<std::string, TypedValue>;
    std::stack<Scope> memoryStack;
    std::unordered_map<std::string, ProcedureNode*> procedures;

    void runLine(const std::string& line);
    void executeLoop(const std::string& var, int from, int to, const std::vector<std::string>& body);
    void callProcedure(const std::string& name, const std::vector<std::string>& args);
    void runThreadedProcedure(const std::string& name);
    void pushScope();
    void popScope();
    std::string getVar(const std::string& name);
    void setVar(const std::string& name, const std::string& value, const std::string& type);
    void reportError(const std::string& msg, int line);
    void inspectMemory();
};

// === Main Compiler and Runner ===
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: quarterc <source.qtr | run file.qtrcapsule | debug file.qtrcapsule>\n";
        return 1;
    }
    std::string mode = argv[1];
    if (mode == "run" && argc >= 3) {
        CapsuleVM vm;
        vm.execute(argv[2]);
        return 0;
    }
    if (mode == "debug" && argc >= 3) {
        CapsuleVM vm;
        vm.execute(argv[2]);
        vm.enterDebugShell();
        return 0;
    }
    std::ifstream file(argv[1]);
    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    ProgramNode* program = parser.parse();
    CodeGenerator generator;
    generator.generate(program);
    NasmCodegen asmgen;
    asmgen.compileToNasm(program);
    CapsuleVM vm;
    vm.execute("output.qtrcapsule");
    return 0;
}
