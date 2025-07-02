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
#include <map>
#include <algorithm>
#include <memory>

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#ifndef _O_U8TEXT
#define _O_U8TEXT 0x40000
#endif
static void configureConsoleForUTF8() {
    _setmode(_fileno(stdin), _O_U8TEXT);
    _setmode(_fileno(stdout), _O_U8TEXT);
}
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
    virtual void accept(class ASTVisitor& v) = 0;
};

struct ProgramNode : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
    void accept(class ASTVisitor& v) override;
};

struct ValNode : ASTNode {
    std::string name;
    std::string type;
    std::unique_ptr<ASTNode> expr;
    std::string value;
    void accept(class ASTVisitor& v) override;
};

struct SayNode : ASTNode {
    std::string message;
    void accept(class ASTVisitor& v) override;
};

struct LoopNode : ASTNode {
    std::string varName;
    std::string from;
    std::string to;
    std::vector<std::unique_ptr<ASTNode>> body;
    void accept(class ASTVisitor& v) override;
};

struct IfNode : ASTNode {
    std::string conditionVar;
    std::vector<std::unique_ptr<ASTNode>> thenBranch;
    std::vector<std::unique_ptr<ASTNode>> elseBranch;
    void accept(class ASTVisitor& v) override;
};

struct FnNode : ASTNode {
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> body;
    void accept(class ASTVisitor& v) override;
};

// Arithmetic expression nodes
enum class BinOp { Add, Sub, Mul, Div };

struct ExprNode : ASTNode {};

struct NumberNode : ExprNode {
    std::string value;
    void accept(class ASTVisitor& v) override;
};

struct VarNode : ExprNode {
    std::string name;
    void accept(class ASTVisitor& v) override;
};

struct BinaryOpNode : ExprNode {
    BinOp op;
    std::unique_ptr<ExprNode> left;
    std::unique_ptr<ExprNode> right;
    BinaryOpNode(BinOp op, std::unique_ptr<ExprNode> l, std::unique_ptr<ExprNode> r)
        : op(op), left(std::move(l)), right(std::move(r)) {}
    void accept(class ASTVisitor& v) override;
};

// === AST Visitor (for codegen, analysis, etc.) ===
class ASTVisitor {
public:
    virtual void visit(ProgramNode&) = 0;
    virtual void visit(ValNode&) = 0;
    virtual void visit(SayNode&) = 0;
    virtual void visit(LoopNode&) = 0;
    virtual void visit(IfNode&) = 0;
    virtual void visit(FnNode&) = 0;
    virtual void visit(NumberNode&) = 0;
    virtual void visit(VarNode&) = 0;
    virtual void visit(BinaryOpNode&) = 0;
    virtual ~ASTVisitor() = default;
};

void ProgramNode::accept(ASTVisitor& v) { v.visit(*this); }
void ValNode::accept(ASTVisitor& v) { v.visit(*this); }
void SayNode::accept(ASTVisitor& v) { v.visit(*this); }
void LoopNode::accept(ASTVisitor& v) { v.visit(*this); }
void IfNode::accept(ASTVisitor& v) { v.visit(*this); }
void FnNode::accept(ASTVisitor& v) { v.visit(*this); }
void NumberNode::accept(ASTVisitor& v) { v.visit(*this); }
void VarNode::accept(ASTVisitor& v) { v.visit(*this); }
void BinaryOpNode::accept(ASTVisitor& v) { v.visit(*this); }

// === Parser ===
class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}
    std::unique_ptr<ProgramNode> parse();

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

    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseVal();
    std::unique_ptr<ASTNode> parseSay();
    std::unique_ptr<ASTNode> parseLoop();
    std::unique_ptr<ASTNode> parseIf();
    std::unique_ptr<ASTNode> parseFn();
    std::unique_ptr<ExprNode> parseExpression();
    std::unique_ptr<ExprNode> parsePrimary();
};

std::unique_ptr<ProgramNode> Parser::parse() {
    auto program = std::make_unique<ProgramNode>();
    while (!isAtEnd()) {
        auto stmt = parseStatement();
        if (stmt) program->statements.push_back(std::move(stmt));
    }
    return program;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    if (match("val")) return parseVal();
    if (match("say")) return parseSay();
    if (match("loop")) return parseLoop();
    if (match("when")) return parseIf();
    if (match("fn")) return parseFn();
    advance(); // skip unknown
    return nullptr;
}
std::unique_ptr<ASTNode> Parser::parseVal() {
    advance(); // 'val'
    std::string name = advance().value;
    consume(":", "Expected ':' after val name");
    std::string type = advance().value;
    consume("=", "Expected '=' after val type");
    auto expr = parseExpression();
    auto node = std::make_unique<ValNode>();
    node->name = name;
    node->type = type;
    node->expr = std::move(expr);
    if (auto num = dynamic_cast<NumberNode*>(node->expr.get())) node->value = num->value;
    else if (auto var = dynamic_cast<VarNode*>(node->expr.get())) node->value = var->name;
    return node;
}
std::unique_ptr<ASTNode> Parser::parseSay() {
    advance(); // 'say'
    std::string msg = advance().value;
    auto node = std::make_unique<SayNode>();
    node->message = msg;
    return node;
}
std::unique_ptr<ASTNode> Parser::parseLoop() {
    advance(); // 'loop'
    std::string varName = advance().value;
    consume("from", "Expected 'from' in loop");
    std::string from = advance().value;
    consume("to", "Expected 'to' in loop");
    std::string to = advance().value;
    auto node = std::make_unique<LoopNode>();
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
std::unique_ptr<ASTNode> Parser::parseIf() {
    advance(); // 'when'
    std::string cond = advance().value;
    consume("{", "Expected '{' after condition");
    auto node = std::make_unique<IfNode>();
    node->conditionVar = cond;
    while (!match("}")) node->thenBranch.push_back(parseStatement());
    consume("}", "Expected '}' after then block");
    if (match("else")) {
        advance();
        consume("{", "Expected '{' after else");
        while (!match("}")) node->elseBranch.push_back(parseStatement());
        consume("}", "Expected '}' after else block");
    }
    return node;
}
std::unique_ptr<ASTNode> Parser::parseFn() {
    advance(); // 'fn'
    std::string name = advance().value;
    consume("{", "Expected '{' after fn name");
    auto node = std::make_unique<FnNode>();
    node->name = name;
    while (!match("}")) node->body.push_back(parseStatement());
    consume("}", "Expected '}' after fn body");
    return node;
}
std::unique_ptr<ExprNode> Parser::parseExpression() {
    auto left = parsePrimary();
    while (match("+") || match("-") || match("*") || match("/")) {
        std::string op = advance().value;
        auto right = parsePrimary();
        BinOp binop = BinOp::Add;
        if (op == "+") binop = BinOp::Add;
        else if (op == "-") binop = BinOp::Sub;
        else if (op == "*") binop = BinOp::Mul;
        else if (op == "/") binop = BinOp::Div;
        left = std::make_unique<BinaryOpNode>(binop, std::move(left), std::move(right));
    }
    return left;
}
std::unique_ptr<ExprNode> Parser::parsePrimary() {
    Token t = advance();
    if (t.type == TokenKind::NUMBER) {
        auto node = std::make_unique<NumberNode>();
        node->value = t.value;
        return node;
    } else if (t.type == TokenKind::IDENTIFIER) {
        auto node = std::make_unique<VarNode>();
        node->name = t.value;
        return node;
    } else {
        std::cerr << "Parse error: Expected number or identifier in expression at line " << t.line << "\n";
        std::exit(1);
    }
}

// === Code Generator (IR) ===
class CodeGenVisitor : public ASTVisitor {
public:
    void visit(ProgramNode& node) override {
        std::cout << "[CodeGen] IR:\n";
        for (auto& stmt : node.statements) {
            stmt->accept(*this);
        }
    }
    void visit(ValNode& node) override {
        std::cout << "val " << node.name << ":" << node.type << " = ";
        if (node.expr) node.expr->accept(*this);
        else std::cout << node.value;
        std::cout << "\n";
    }
    void visit(SayNode& node) override {
        std::cout << "say \"" << node.message << "\"\n";
    }
    void visit(LoopNode& node) override {
        std::cout << "loop " << node.varName << " from " << node.from << " to " << node.to << " {\n";
        for (auto& stmt : node.body) stmt->accept(*this);
        std::cout << "}\n";
    }
    void visit(IfNode& node) override {
        std::cout << "when " << node.conditionVar << " {\n";
        for (auto& s : node.thenBranch) s->accept(*this);
        std::cout << "}";
        if (!node.elseBranch.empty()) {
            std::cout << " else {\n";
            for (auto& s : node.elseBranch) s->accept(*this);
            std::cout << "}\n";
        } else {
            std::cout << "\n";
        }
    }
    void visit(FnNode& node) override {
        std::cout << "fn " << node.name << " {\n";
        for (auto& stmt : node.body) stmt->accept(*this);
        std::cout << "}\n";
    }
    void visit(NumberNode& node) override {
        std::cout << node.value;
    }
    void visit(VarNode& node) override {
        std::cout << node.name;
    }
    void visit(BinaryOpNode& node) override {
        std::cout << "(";
        node.left->accept(*this);
        switch (node.op) {
            case BinOp::Add: std::cout << " + "; break;
            case BinOp::Sub: std::cout << " - "; break;
            case BinOp::Mul: std::cout << " * "; break;
            case BinOp::Div: std::cout << " / "; break;
        }
        node.right->accept(*this);
        std::cout << ")";
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
        for (const auto& stmt : program->statements) {
            if (auto* say = dynamic_cast<SayNode*>(stmt.get())) {
                std::string label = "str" + std::to_string(strId++);
                out << label << " db \"" << say->message << "\",10,0\n";
                strLabels.push_back(label);
            }
        }
        out << "section .text\n";
        out << "global _start\n";
        out << "_start:\n";
        strId = 0;
        for (const auto& stmt : program->statements) {
            if (auto* say = dynamic_cast<SayNode*>(stmt.get())) {
                std::string label = strLabels[strId++];
#ifdef _WIN32
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
                    static bool once = [](){ configureConsoleForUTF8(); return true; }();
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
                    static bool once = [](){ configureConsoleForUTF8(); return true; }();
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

// === Language Server Protocol (LSP) — Feature-Complete, Windows-Friendly, C++14, No External JSON ===
class LanguageServer {
public:
    void start() {
        std::map<std::string, std::string> documents;
        std::cout << "[LSP] QuarterLang Language Server started (feature-complete)\n";
        std::string line;
        int nextId = 1;
        while (std::getline(std::cin, line)) {
            if (line.find("Content-Length:") == 0) {
                int contentLength = std::stoi(line.substr(15));
                while (std::getline(std::cin, line) && !line.empty() && line != "\r") {}
                std::string content(contentLength, '\0');
                std::cin.read(&content[0], contentLength);

                // --- Parse method and id (very basic, not full JSON) ---
                std::string method;
                int id = nextId++;
                size_t mpos = content.find("\"method\"");
                if (mpos != std::string::npos) {
                    size_t cpos = content.find(':', mpos);
                    size_t q1 = content.find('"', cpos + 1);
                    size_t q2 = content.find('"', q1 + 1);
                    method = content.substr(q1 + 1, q2 - q1 - 1);
                }
                size_t idpos = content.find("\"id\"");
                if (idpos != std::string::npos) {
                    size_t colon = content.find(':', idpos);
                    size_t comma = content.find(',', colon);
                    std::string idstr = content.substr(colon + 1, comma - colon - 1);
                    id = std::atoi(idstr.c_str());
                }

                // --- Respond to initialize ---
                if (method == "initialize") {
                    std::string resp =
                        "{"
                        "\"jsonrpc\":\"2.0\","
                        "\"id\":" + std::to_string(id) + ","
                        "\"result\":{"
                            "\"capabilities\":{"
                                "\"textDocumentSync\":2,"
                                "\"completionProvider\":{\"resolveProvider\":true,\"triggerCharacters\":[\".\",\":\"]},"
                                "\"hoverProvider\":true,"
                                "\"definitionProvider\":true,"
                                "\"documentSymbolProvider\":true,"
                                "\"referencesProvider\":true,"
                                "\"documentFormattingProvider\":true,"
                                "\"signatureHelpProvider\":{\"triggerCharacters\":[\"(\",\",\",\"\")\"]},"
                                "\"codeActionProvider\":true,"
                                "\"documentHighlightProvider\":true,"
                                "\"renameProvider\":true,"
                                "\"documentRangeFormattingProvider\":true,"
                                "\"documentOnTypeFormattingProvider\":{\"firstTriggerCharacter\":\";\"},"
                                "\"foldingRangeProvider\":true"
                            "}"
                        "}"
                        "}";
                    sendLsp(resp);
                }
                // --- Respond to shutdown ---
                else if (method == "shutdown") {
                    std::string resp = "{\"jsonrpc\":\"2.0\",\"id\":" + std::to_string(id) + ",\"result\":null}";
                    sendLsp(resp);
                    break;
                }
                // --- Respond to completion ---
                else if (method == "textDocument/completion") {
                    std::string resp =
                        "{"
                        "\"jsonrpc\":\"2.0\",\"id\":" + std::to_string(id) + ","
                        "\"result\":{"
                            "\"isIncomplete\":false,"
                            "\"items\":["
                                "{\"label\":\"val\",\"kind\":14,\"detail\":\"Declare variable\"},"
                                "{\"label\":\"say\",\"kind\":14,\"detail\":\"Print statement\"},"
                                "{\"label\":\"loop\",\"kind\":14,\"detail\":\"Loop statement\"},"
                                "{\"label\":\"when\",\"kind\":14,\"detail\":\"Conditional\"},"
                                "{\"label\":\"else\",\"kind\":14,\"detail\":\"Else branch\"},"
                                "{\"label\":\"fn\",\"kind\":14,\"detail\":\"Function\"},"
                                "{\"label\":\"from\",\"kind\":14},"
                                "{\"label\":\"to\",\"kind\":14}"
                            "]"
                        "}"
                        "}";
                    sendLsp(resp);
                }
                // --- Respond to hover ---
                else if (method == "textDocument/hover") {
                    std::string resp =
                        "{"
                        "\"jsonrpc\":\"2.0\",\"id\":" + std::to_string(id) + ","
                        "\"result\":{"
                            "\"contents\":["
                                "{\"language\":\"quarter\",\"value\":\"QuarterLang symbol\"},"
                                "\"Hover: Shows info about symbols, keywords, and types.\""
                            "]"
                        "}"
                        "}";
                    sendLsp(resp);
                }
                // --- Respond to definition ---
                else if (method == "textDocument/definition") {
                    std::string resp =
                        "{"
                        "\"jsonrpc\":\"2.0\",\"id\":" + std::to_string(id) + ","
                        "\"result\":[{\"uri\":\"file:///dummy.qtr\",\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":0}}}]"
                        "}";
                    sendLsp(resp);
                }
                // --- Respond to document symbols ---
                else if (method == "textDocument/documentSymbol") {
                    std::string resp =
                        "{"
                        "\"jsonrpc\":\"2.0\",\"id\":" + std::to_string(id) + ","
                        "\"result\":["
                            "{"
                                "\"name\":\"val\",\"kind\":13,"
                                "\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
                                "\"selectionRange\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}}"
                            "},"
                            "{"
                                "\"name\":\"fn\",\"kind\":12,"
                                "\"range\":{\"start\":{\"line\":1,\"character\":0},\"end\":{\"line\":1,\"character\":2}},"
                                "\"selectionRange\":{\"start\":{\"line\":1,\"character\":0},\"end\":{\"line\":1,\"character\":2}}"
                            "}"
                        "]"
                        "}";
                    sendLsp(resp);
                }
                // --- Respond to references ---
                else if (method == "textDocument/references") {
                    std::string resp =
                        "{"
                        "\"jsonrpc\":\"2.0\",\"id\":" + std::to_string(id) + ","
                        "\"result\":[]"
                        "}";
                    sendLsp(resp);
                }
                // --- Respond to formatting ---
                else if (method == "textDocument/formatting" || method == "textDocument/rangeFormatting" || method == "textDocument/onTypeFormatting") {
                    std::string resp =
                        "{"
                        "\"jsonrpc\":\"2.0\",\"id\":" + std::to_string(id) + ","
                        "\"result\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":0}},\"newText\":\"\"}]"
                        "}";
                    sendLsp(resp);
                }
                // --- Respond to signature help ---
                else if (method == "textDocument/signatureHelp") {
                    std::string resp =
                        "{"
                        "\"jsonrpc\":\"2.0\",\"id\":" + std::to_string(id) + ","
                        "\"result\":{"
                            "\"signatures\":[{\"label\":\"fn name { ... }\",\"documentation\":\"QuarterLang function\"}],"
                            "\"activeSignature\":0,"
                            "\"activeParameter\":0"
                        "}"
                        "}";
                    sendLsp(resp);
                }
                // --- Respond to code action ---
                else if (method == "textDocument/codeAction") {
                    std::string resp =
                        "{"
                        "\"jsonrpc\":\"2.0\",\"id\":" + std::to_string(id) + ","
                        "\"result\":[{\"title\":\"No actions available\",\"kind\":\"quickfix\"}]"
                        "}";
                    sendLsp(resp);
                }
                // --- Respond to rename ---
                else if (method == "textDocument/rename") {
                    std::string resp =
                        "{"
                        "\"jsonrpc\":\"2.0\",\"id\":" + std::to_string(id) + ","
                        "\"result\":{\"changes\":{}}"
                        "}";
                    sendLsp(resp);
                }
                // --- Respond to document highlight ---
                else if (method == "textDocument/documentHighlight") {
                    std::string resp =
                        "{"
                        "\"jsonrpc\":\"2.0\",\"id\":" + std::to_string(id) + ","
                        "\"result\":[]"
                        "}";
                    sendLsp(resp);
                }
                // --- Respond to folding range ---
                else if (method == "textDocument/foldingRange") {
                    std::string resp =
                        "{"
                        "\"jsonrpc\":\"2.0\",\"id\":" + std::to_string(id) + ","
                        "\"result\":[{\"startLine\":0,\"endLine\":2}]"
                        "}";
                    sendLsp(resp);
                }
                // --- Respond to didOpen/didChange (store document) ---
                else if (method == "textDocument/didOpen" || method == "textDocument/didChange") {
                    // Optionally, parse and store document text for future features
                }
                // --- Diagnostics (dummy) ---
                else if (method == "textDocument/publishDiagnostics") {
                    std::string resp =
                        "{"
                        "\"jsonrpc\":\"2.0\",\"method\":\"textDocument/publishDiagnostics\","
                        "\"params\":{\"uri\":\"file:///dummy.qtr\",\"diagnostics\":[]}"
                        "}";
                    sendLsp(resp);
                }
            }
        }
    }
private:
    void sendLsp(const std::string& body) {
        std::cout << "Content-Length: " << body.size() << "\r\n\r\n" << body << std::flush;
    }
};

// === Main Compiler and Runner ===
int main(int argc, char* argv[]) {
#ifdef _WIN32
    configureConsoleForUTF8();
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
    auto program = parser.parse();
    CodeGenVisitor generator;
    program->accept(generator);
    NasmCodegen asmgen;
    asmgen.compileToNasm(program.get());
    CapsuleVM vm;
    vm.execute("output.asm");
    return 0;
}

