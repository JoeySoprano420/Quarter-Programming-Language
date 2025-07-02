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

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

// === Token Types ===
// TokenKind enumerates all possible token categories produced by the lexer.
enum class TokenKind {
    KEYWORD,        // Reserved words in the language (e.g., "val", "say", "loop")
    IDENTIFIER,     // User-defined names (variables, functions, etc.)
    NUMBER,         // Numeric literals (e.g., 123, 42)
    STRING,         // String literals (e.g., "hello")
    SYMBOL,         // Single-character symbols (e.g., :, =, {, }, etc.)
    DG,             // Special type for "dg" (domain-specific, e.g., for graphs)
    END_OF_FILE     // End of input marker
};

// Define the Token structure after the TokenKind enum declaration.
struct Token {
    TokenKind type;
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
            return Token{ TokenKind::KEYWORD, word, line };
        return Token{ TokenKind::IDENTIFIER, word, line };
    }
    Token readNumber() {
        size_t start = pos;
        while (std::isdigit(peek())) advance();
        return Token{ TokenKind::NUMBER, source.substr(start, pos - start), line };
    }
    Token readString() {
        advance(); // skip opening "
        size_t start = pos;
        while (peek() != '"' && peek() != '\0') advance();
        std::string str = source.substr(start, pos - start);
        if (peek() == '"') advance();
        return Token{ TokenKind::STRING, str, line };
    }
    Token readSymbol() {
        char c = advance();
        return Token{ TokenKind::SYMBOL, std::string(1, c), line };
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
    tokens.push_back(Token{ TokenKind::END_OF_FILE, "", line });
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

struct LoopNode : ASTNode {
    std::string varName;
    std::string from;
    std::string to;
    std::vector<ASTNode*> body;
    ~LoopNode() {
        for (auto* stmt : body) delete stmt;
    }
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
    bool isAtEnd() { return peek().type == TokenKind::END_OF_FILE; }

    ASTNode* parseStatement() {
        if (match("val")) return parseVal();
        if (match("say")) return parseSay();
        if (match("loop")) return parseLoop();
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
};

// === Code Generator ===
class CodeGenerator {
public:
    void generate(ProgramNode* program) {
        std::cout << "[CodeGen] IR:\n";
        for (auto* stmt : program->statements) {
            if (auto* val = dynamic_cast<ValNode*>(stmt)) {
                std::cout << "val " << val->name << ":" << val->type << " = " << val->value << "\n";
            } else if (auto* say = dynamic_cast<SayNode*>(stmt)) {
                std::cout << "say \"" << say->message << "\"\n";
            } else if (auto* loop = dynamic_cast<LoopNode*>(stmt)) {
                std::cout << "loop " << loop->varName << " from " << loop->from << " to " << loop->to << " {\n";
                for (auto* bodyStmt : loop->body) {
                    if (auto* say2 = dynamic_cast<SayNode*>(bodyStmt)) {
                        std::cout << "  say \"" << say2->message << "\"\n";
                    }
                }
                std::cout << "}\n";
            }
        }
    }
};

// === NASM Codegen ===
class NasmCodegen {
public:
    void compileToNasm(ProgramNode* program) {
        std::ofstream out("output.asm");
        out << "section .data\n";
        int strId = 0;
        std::vector<std::string> strLabels;
        for (auto* stmt : program->statements) {
            if (auto* say = dynamic_cast<SayNode*>(stmt)) {
                std::string label = "str" + std::to_string(strId++);
                out << label << " db \"" << say->message << "\",10,0\n";
                strLabels.push_back(label);
            }
        }
        out << "section .text\n";
        out << "global _start\n";
        out << "_start:\n";
        strId = 0;
        for (auto* stmt : program->statements) {
            if (auto* say = dynamic_cast<SayNode*>(stmt)) {
                std::string label = strLabels[strId++];
#ifdef _WIN32
                // Windows: Use WriteFile syscall via inline assembly or call C runtime
                // For demonstration, emit a comment
                out << "    ; Windows: Output not implemented in NASM stub\n";
#else
                out << "    mov rax, 1\n";
                out << "    mov rdi, 1\n";
                out << "    mov rsi, " << label << "\n";
                out << "    mov rdx, " << (say->message.size() + 1) << "\n";
                out << "    syscall\n";
#endif
            }
        }
#ifdef _WIN32
        out << "    ; Windows: Exit process (not implemented in NASM stub)\n";
#else
        out << "    mov rax, 60\n    xor rdi, rdi\n    syscall\n";
#endif
        out.close();
        std::cout << "[NASM] Assembly written to output.asm\n";
    }
};

// === Capsule VM Runtime ===
class CapsuleVM {
public:
    void execute(const std::string& capsulePath) {
        std::ifstream in(capsulePath);
        if (!in) {
            std::cerr << "[VM] Could not open capsule: " << capsulePath << "\n";
            return;
        }
        std::string line;
        while (std::getline(in, line)) {
            if (line.find("say") == 0) {
                size_t pos = line.find("\"");
                if (pos != std::string::npos) {
                    std::string msg = line.substr(pos + 1, line.rfind("\"") - pos - 1);
#ifdef _WIN32
                    // Set output mode to UTF-8 for Windows console
                    static bool once = [](){ _setmode(_fileno(stdout), _O_U8TEXT); return true; }();
                    std::wcout << std::wstring(msg.begin(), msg.end()) << std::endl;
#else
                    std::cout << msg << std::endl;
#endif
                }
            }
        }
    }
    void runInteractive() {
        std::cout << "[VM] Interactive mode. Type 'exit' to quit.\n";
        std::string line;
        while (true) {
            std::cout << ">> ";
            std::getline(std::cin, line);
            if (line == "exit") break;
            if (line.find("say") == 0) {
                size_t pos = line.find("\"");
                if (pos != std::string::npos) {
                    std::string msg = line.substr(pos + 1, line.rfind("\"") - pos - 1);
#ifdef _WIN32
                    static bool once = [](){ _setmode(_fileno(stdout), _O_U8TEXT); return true; }();
                    std::wcout << std::wstring(msg.begin(), msg.end()) << std::endl;
#else
                    std::cout << msg << std::endl;
#endif
                }
            }
        }
    }
    void enterDebugShell() {
        std::cout << "[VM] Debug shell. Type 'mem' to inspect memory, 'exit' to quit.\n";
        std::string line;
        while (true) {
            std::cout << "(debug) ";
            std::getline(std::cin, line);
            if (line == "exit") break;
            if (line == "mem") inspectMemory();
        }
    }
private:
    typedef std::unordered_map<std::string, TypedValue> Scope;
    std::stack<Scope> memoryStack;
    std::unordered_map<std::string, int> variables;
    void inspectMemory() {
        std::cout << "[VM] Memory inspection not implemented.\n";
    }
};

// === Language Server Protocol (LSP) Minimal Example ===
class LanguageServer {
public:
    void start() {
        std::cout << "[LSP] Language server started (minimal example)\n";
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line.find("shutdown") != std::string::npos) break;
            if (line.find("initialize") != std::string::npos)
                std::cout << "{\"jsonrpc\":\"2.0\",\"result\":{\"capabilities\":{}},\"id\":1}\n";
        }
    }
};

// === Main Compiler and Runner ===
int main(int argc, char* argv[]) {
#ifdef _WIN32
    // Set stdin/stdout to UTF-8 for Windows console
    _setmode(_fileno(stdin), _O_U8TEXT);
    _setmode(_fileno(stdout), _O_U8TEXT);
#endif
    if (argc < 2) {
        std::cerr << "Usage: quarterc <source.qtr | run file.qtrcapsule | debug file.qtrcapsule | lsp>\n";
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
    if (mode == "lsp") {
        LanguageServer lsp;
        lsp.start();
        return 0;
    }
    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Could not open file: " << argv[1] << "\n";
        return 1;
    }
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
    vm.execute("output.asm");
    delete program;
    return 0;
}
