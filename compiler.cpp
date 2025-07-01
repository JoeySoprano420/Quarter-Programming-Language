// QUARTERLANG SUPERCOMPILER v1.0 - FULL ALL-IN-ONE
// (C) 2025 Violet Aura Creations | MIT License

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <variant>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <thread>
#include <mutex>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#define popen _popen
#define pclose _pclose
#endif

// -------- LANGUAGE DEFINITIONS --------

enum class TokenType {
    IDENT, KEYWORD, NUMBER, STRING, SYMBOL, NEWLINE, END
};

struct Token {
    TokenType type;
    std::string text;
    int line;
};

const std::unordered_set<std::string> keywords = {
    "star", "end", "define", "val", "var", "loop", "from", "to", "when", "else", "say", "procedure",
    "return", "asm", "nest", "dg", "stop", "match", "case", "pipe", "tuple", "fn", "yield", "thread"
};

// -------- TOKENIZER --------

class Tokenizer {
    std::string src;
    size_t i = 0;
    int line = 1;
    std::vector<Token> tokens;

    void skipWhitespace() {
        while (i < src.size() && isspace(src[i])) {
            if (src[i] == '\n') ++line;
            ++i;
        }
    }

public:
    Tokenizer(const std::string& s) : src(s) {}
    std::vector<Token> run() {
        while (i < src.size()) {
            skipWhitespace();
            if (i >= src.size()) break;
            if (isalpha(src[i]) || src[i] == '_') {
                size_t start = i;
                while (i < src.size() && (isalnum(src[i]) || src[i] == '_')) ++i;
                std::string word = src.substr(start, i - start);
                TokenType ttype = keywords.count(word) ? TokenType::KEYWORD : TokenType::IDENT;
                tokens.push_back({ttype, word, line});
            } else if (isdigit(src[i])) {
                size_t start = i;
                while (i < src.size() && isdigit(src[i])) ++i;
                tokens.push_back({TokenType::NUMBER, src.substr(start, i - start), line});
            } else if (src[i] == '"') {
                ++i;
                size_t start = i;
                while (i < src.size() && src[i] != '"') ++i;
                tokens.push_back({TokenType::STRING, src.substr(start, i - start), line});
                ++i;
            } else if (src[i] == '\n') {
                tokens.push_back({TokenType::NEWLINE, "\\n", line++});
                ++i;
            } else {
                tokens.push_back({TokenType::SYMBOL, std::string(1, src[i]), line});
                ++i;
            }
        }
        tokens.push_back({TokenType::END, "", line});
        return tokens;
    }
};

// -------- AST NODE --------

struct ASTNode;
using AST = std::unique_ptr<ASTNode>;

struct ASTNode {
    std::string type;              // "say", "val", "match", "pipe", etc.
    std::string name;              // for identifiers
    std::vector<std::string> args; // argument literals
    std::vector<AST> children;     // block body
    std::unordered_map<std::string, std::string> meta; // for nests, dgs, etc.
};

// -------- PARSER --------

class Parser {
    std::vector<Token> toks;
    size_t idx = 0;

    Token& peek(int k=0) { return toks[std::min(idx+k, toks.size()-1)]; }
    Token& next() { return toks[std::min(idx++, toks.size()-1)]; }

    bool match(const std::string& txt) {
        return peek().text == txt;
    }
    bool matchType(TokenType t) {
        return peek().type == t;
    }
    void expect(const std::string& txt) {
        if (!match(txt)) { error("Expected '"+txt+"'"); }
    }
    void error(const std::string& msg) {
        std::cerr << "[Quarter][ParseError] " << msg << " at line " << peek().line << "\n";
        exit(2);
    }

public:
    Parser(const std::vector<Token>& t) : toks(t) {}
    AST parse() {
        AST root = std::make_unique<ASTNode>();
        root->type = "program";
        while (!matchType(TokenType::END)) {
            auto stmt = parseStatement();
            if (stmt) root->children.push_back(std::move(stmt));
        }
        return root;
    }

    AST parseStatement() {
        if (match("say")) {
            next(); // 'say'
            AST node = std::make_unique<ASTNode>();
            node->type = "say";
            if (matchType(TokenType::STRING) || matchType(TokenType::IDENT) || matchType(TokenType::NUMBER))
                node->args.push_back(next().text);
            return node;
        }
        if (match("val") || match("var")) {
            bool isConst = match("val");
            next();
            std::string name = next().text;
            expect("as"); next();
            std::string ty = next().text;
            expect(":"); next();
            std::string val = next().text;
            AST node = std::make_unique<ASTNode>();
            node->type = isConst ? "val" : "var";
            node->name = name;
            node->args = {ty, val};
            return node;
        }
        if (match("loop")) {
            next();
            std::string item = next().text;
            expect("from"); next();
            std::string start = next().text;
            expect("to"); next();
            std::string end = next().text;
            AST node = std::make_unique<ASTNode>();
            node->type = "loop";
            node->name = item;
            node->args = {start, end};
            while (!match("stop")) {
                node->children.push_back(parseStatement());
            }
            next(); // stop
            return node;
        }
        if (match("match")) {
            next();
            std::string target = next().text;
            AST node = std::make_unique<ASTNode>();
            node->type = "match";
            node->name = target;
            while (match("case")) {
                next();
                std::string value = next().text;
                expect(":"); next();
                AST branch = std::make_unique<ASTNode>();
                branch->type = "case";
                branch->name = value;
                branch->children.push_back(parseStatement());
                node->children.push_back(std::move(branch));
            }
            if (match("else")) {
                next(); expect(":"); next();
                AST branch = std::make_unique<ASTNode>();
                branch->type = "else";
                branch->children.push_back(parseStatement());
                node->children.push_back(std::move(branch));
            }
            expect("end"); next();
            return node;
        }
        if (match("pipe")) {
            next();
            AST node = std::make_unique<ASTNode>();
            node->type = "pipe";
            node->name = next().text;
            // more parsing for chained pipes if needed
            return node;
        }
        if (match("tuple")) {
            next();
            AST node = std::make_unique<ASTNode>();
            node->type = "tuple";
            node->args.push_back(next().text); // e.g. "a", or "(a,b)"
            if (match(":")) { next(); node->args.push_back(next().text); }
            return node;
        }
        if (match("dg")) {
            next();
            AST node = std::make_unique<ASTNode>();
            node->type = "dg";
            node->args.push_back(next().text); // binary value
            return node;
        }
        if (match("asm")) {
            next();
            expect("{"); next();
            AST node = std::make_unique<ASTNode>();
            node->type = "asm";
            std::ostringstream block;
            while (!match("}")) { block << next().text << " "; }
            next();
            node->args.push_back(block.str());
            return node;
        }
        if (match("procedure")) {
            next();
            std::string pname = next().text;
            AST node = std::make_unique<ASTNode>();
            node->type = "procedure";
            node->name = pname;
            while (!match("end")) {
                node->children.push_back(parseStatement());
            }
            next();
            return node;
        }
        if (matchType(TokenType::NEWLINE)) { next(); return nullptr; }
        next(); return nullptr;
    }
};

// -------- CAPSULE OUTPUT (Stub) --------

void write_capsule(const std::string& bin, const std::string& asmCode) {
    std::ofstream f("out.qtrcapsule", std::ios::binary);
    f.write(bin.c_str(), bin.size());
    f.close();
    // Capsule could contain meta, NASM, trace, and compressed code
}

// -------- NASM EMITTER --------

class NasmEmitter {
public:
    std::ostringstream data;
    std::ostringstream text;
    int strID = 0;

    NasmEmitter() {
        data << "section .data\n";
        text << "section .text\n    global _start\n_start:\n";
    }

    void emitSay(const std::string& msg) {
        std::string label = "str_" + std::to_string(strID++);
        data << label << " db \"" << msg << "\", 10\n";
        data << label << "_len equ $ - " << label << "\n";
        text << "    mov rax, 1\n";
        text << "    mov rdi, 1\n";
        text << "    mov rsi, " << label << "\n";
        text << "    mov rdx, " << label << "_len\n";
        text << "    syscall\n";
    }

    void emitLoop(const ASTNode& loop) {
        std::string loopvar = loop.name;
        std::string loop_start = loop.args[0];
        std::string loop_end = loop.args[1];
        text << "    mov ecx, " << loop_start << "\n";
        text << loopvar << "_loop:\n";
        text << "    cmp ecx, " << loop_end << "\n";
        text << "    jg " << loopvar << "_end\n";
        for (const auto& stmt : loop.children)
            if (stmt->type == "say") emitSay(stmt->args[0]);
        text << "    inc ecx\n";
        text << "    jmp " << loopvar << "_loop\n";
        text << loopvar << "_end:\n";
    }

    void emitMatch(const ASTNode& match) {
        std::string matchvar = match.name;
        std::string matchexpr = "rax";
        text << "    mov " << matchexpr << ", [" << matchvar << "]\n";
        int caseN = 0;
        for (const auto& branch : match.children) {
            if (branch->type == "case") {
                text << "    cmp " << matchexpr << ", " << branch->name << "\n";
                text << "    je match_case_" << caseN << "\n";
            }
            ++caseN;
        }
        text << "    jmp match_else\n";
        caseN = 0;
        for (const auto& branch : match.children) {
            if (branch->type == "case") {
                text << "match_case_" << caseN << ":\n";
                for (const auto& s : branch->children)
                    if (s->type == "say") emitSay(s->args[0]);
                text << "    jmp match_end\n";
            }
            ++caseN;
        }
        text << "match_else:\n";
        for (const auto& branch : match.children)
            if (branch->type == "else")
                for (const auto& s : branch->children)
                    if (s->type == "say") emitSay(s->args[0]);
        text << "match_end:\n";
    }

    void emitAsmInline(const std::string& code) {
        text << "; [inline NASM]\n" << code << "\n";
    }

    void emit(const AST& ast) {
        for (const auto& stmt : ast->children) {
            if (stmt->type == "say") emitSay(stmt->args[0]);
            if (stmt->type == "loop") emitLoop(*stmt);
            if (stmt->type == "match") emitMatch(*stmt);
            if (stmt->type == "asm") emitAsmInline(stmt->args[0]);
            // TODO: pipe, tuple, dg, thread, etc.
        }
        text << "    mov rax, 60\n    xor rdi, rdi\n    syscall\n";
    }

    std::string fullASM() const {
        return data.str() + "\n" + text.str();
    }
};

// -------- DEBUGGER --------

void launch_debugger(const std::string& asmCode) {
    std::cout << "[Quarter][Debugger] (Stub):\n";
    std::cout << "---- NASM Output ----\n";
    std::istringstream s(asmCode);
    std::string line;
    int lineno = 1;
    while (std::getline(s, line))
        std::cout << lineno++ << ": " << line << "\n";
    // Could add live breakpoints, variable watch, etc.
    std::cout << "---------------------\n";
}

// -------- VISUALIZER (Stub) --------

void launch_visualizer(const AST& ast) {
    std::cout << "[Quarter][Visualizer] (Stub):\n";
    std::function<void(const AST&, int)> printTree = [&](const AST& n, int d) {
        if (!n) return;
        std::cout << std::string(d * 2, ' ') << n->type;
        if (!n->name.empty()) std::cout << " (" << n->name << ")";
        for (auto& a : n->args) std::cout << " [" << a << "]";
        std::cout << "\n";
        for (const auto& c : n->children) printTree(c, d+1);
    };
    printTree(ast, 0);
}

// -------- AI CODEGEN PLUGIN (Stub) --------

std::string ai_generate_code(const std::string& prompt) {
    // Placeholder: In production, connect to QuarterGPT or local LLM API
    if (prompt.find("hello") != std::string::npos)
        return "star\nsay \"Hello from AI\"\nend\n";
    return "";
}

// -------- MAIN ENTRY --------

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: quarterc <file.qtr> [--debug] [--viz] [--ai \"prompt\"]\n";
        return 1;
    }
    std::string filename = argv[1];
    bool debug = false, viz = false, ai_mode = false;
    std::string ai_prompt;
    for (int i=2;i<argc;++i) {
        if (std::string(argv[i]) == "--debug") debug = true;
        if (std::string(argv[i]) == "--viz") viz = true;
        if (std::string(argv[i]) == "--ai" && i+1<argc) { ai_mode = true; ai_prompt = argv[++i]; }
    }
    std::string code;
    if (ai_mode) {
        code = ai_generate_code(ai_prompt);
        std::ofstream("ai_gen.qtr") << code;
        filename = "ai_gen.qtr";
    }
    std::ifstream file(filename);
    if (!file.is_open()) { std::cerr << "Can't open file.\n"; return 1; }
    if (!ai_mode)
        code.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    Tokenizer lexer(code);
    auto tokens = lexer.run();
    Parser parser(tokens);
    auto ast = parser.parse();

    if (viz) { launch_visualizer(ast); return 0; }

    NasmEmitter emitter;
    emitter.emit(ast);
    auto asmCode = emitter.fullASM();

    if (debug) { launch_debugger(asmCode); return 0; }

    std::ofstream("out.asm") << asmCode;
    system("nasm -f elf64 out.asm -o out.o");
    system("ld out.o -o out");
    std::cout << "[Quarter] Compilation complete. Run with ./out\n";
    write_capsule("CAPSULE_BINARY_STUB", asmCode);

    return 0;
}

// QUARTERLANG SUPERCOMPILER v2.0 - EXTENDED WITH COMPRESSION + CAPSULE FORMAT + AI INTERFACE
// (C) 2025 Violet Aura Creations | MIT License

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <variant>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <zlib.h> // For compression
#include <thread>
#include <mutex>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#define popen _popen
#define pclose _pclose
#endif

// The rest of the compiler implementation would go here.
// For brevity, assume we include:
// - Tokenizer class
// - Parser class
// - AST definition
// - NASM Emitter
// - Capsule Writer (with compression using zlib)
// - Visualizer
// - Debugger
// - QuarterGPT Local Bridge (stubbed with command or socket interface)

int compressData(const std::string& input, std::string& output) {
    uLongf compressedSize = compressBound(input.size());
    std::vector<Bytef> buffer(compressedSize);
    int res = compress(buffer.data(), &compressedSize, (const Bytef*)input.data(), input.size());
    if (res != Z_OK) return res;
    output.assign((char*)buffer.data(), compressedSize);
    return Z_OK;
}

void writeQuarterCapsule(const std::string& asmCode, const std::string& rawSource) {
    std::string compressedAsm, compressedSrc;
    compressData(asmCode, compressedAsm);
    compressData(rawSource, compressedSrc);
    
    std::ofstream out("out.qtrcapsule", std::ios::binary);
    if (!out) { std::cerr << "Failed to write capsule.\n"; return; }

    // Simple header
    out.write("QTRC", 4);
    uint32_t version = 1;
    out.write(reinterpret_cast<const char*>(&version), sizeof(version));

    // Section sizes
    uint32_t asmSize = compressedAsm.size();
    uint32_t srcSize = compressedSrc.size();
    out.write(reinterpret_cast<const char*>(&asmSize), sizeof(asmSize));
    out.write(reinterpret_cast<const char*>(&srcSize), sizeof(srcSize));

    // Section contents
    out.write(compressedAsm.data(), compressedAsm.size());
    out.write(compressedSrc.data(), compressedSrc.size());

    out.close();
    std::cout << "[Quarter] Capsule written: out.qtrcapsule (" << compressedAsm.size() + compressedSrc.size() << " bytes)\n";
}

// Stub for AI integration
std::string queryQuarterGPTLocal(const std::string& prompt) {
    std::cout << "[QuarterGPT-Local] Prompt: " << prompt << "\n";
    return "star\nsay \"AI Response\"\nend\n";
}

int main() {
    // This main function will be replaced with the full main loop
    // from the previous compiler version, extended to call:
    // - writeQuarterCapsule()
    // - queryQuarterGPTLocal()
    // with actual source and output
    std::cout << "[Quarter] Bootstrapped compiler with compression and AI interface.\n";
    return 0;
}

// === Token Representation ===
struct Token {
    enum Type { IDENTIFIER, KEYWORD, NUMBER, STRING, SYMBOL, END } type;
    std::string text;
};

// === Keywords ===
const std::unordered_set<std::string> keywords = {
    "star", "end", "say", "val", "var", "loop", "from", "to", "match", "case", "else", "dg", "asm"
};

// === Tokenizer ===
std::vector<Token> tokenize(const std::string& source) {
    std::vector<Token> tokens;
    size_t i = 0;
    while (i < source.size()) {
        if (isspace(source[i])) { ++i; continue; }
        if (isalpha(source[i])) {
            size_t start = i;
            while (isalnum(source[i]) || source[i] == '_') ++i;
            std::string word = source.substr(start, i - start);
            tokens.push_back({ keywords.count(word) ? Token::KEYWORD : Token::IDENTIFIER, word });
        } else if (isdigit(source[i])) {
            size_t start = i;
            while (isdigit(source[i])) ++i;
            tokens.push_back({ Token::NUMBER, source.substr(start, i - start) });
        } else if (source[i] == '"') {
            size_t start = ++i;
            while (i < source.size() && source[i] != '"') ++i;
            tokens.push_back({ Token::STRING, source.substr(start, i - start) });
            ++i;
        } else {
            tokens.push_back({ Token::SYMBOL, std::string(1, source[i++]) });
        }
    }
    tokens.push_back({ Token::END, "" });
    return tokens;
}

// === AST Node ===
struct ASTNode {
    std::string type;
    std::string value;
    std::vector<std::unique_ptr<ASTNode>> children;
};

// === Parser ===
std::unique_ptr<ASTNode> parseSay(const std::vector<Token>& tokens, size_t& pos) {
    auto node = std::make_unique<ASTNode>();
    node->type = "say";
    ++pos;
    if (tokens[pos].type == Token::STRING) {
        node->value = tokens[pos].text;
        ++pos;
    }
    return node;
}

std::unique_ptr<ASTNode> parseVal(const std::vector<Token>& tokens, size_t& pos) {
    auto node = std::make_unique<ASTNode>();
    node->type = "val";
    ++pos;
    if (tokens[pos].type == Token::IDENTIFIER) {
        node->value = tokens[pos].text;
        ++pos;
    }
    return node;
}

std::unique_ptr<ASTNode> parse(const std::vector<Token>& tokens) {
    size_t pos = 0;
    auto root = std::make_unique<ASTNode>();
    root->type = "program";
    while (tokens[pos].type != Token::END) {
        if (tokens[pos].text == "say") {
            root->children.push_back(parseSay(tokens, pos));
        } else if (tokens[pos].text == "val") {
            root->children.push_back(parseVal(tokens, pos));
        } else {
            ++pos;
        }
    }
    return root;
}

// === NASM Emitter ===
std::string emitASM(const std::unique_ptr<ASTNode>& root) {
    std::stringstream asmCode;
    asmCode << "section .data\n";
    for (const auto& child : root->children) {
        if (child->type == "say") {
            asmCode << "msg db \"" << child->value << "\",10\n";
            asmCode << "msg_len equ $ - msg\n";
        }
    }
    asmCode << "section .text\nglobal _start\n_start:\n";
    for (const auto& child : root->children) {
        if (child->type == "say") {
            asmCode << "mov rax, 1\nmov rdi, 1\nmov rsi, msg\nmov rdx, msg_len\nsyscall\n";
        }
    }
    asmCode << "mov rax, 60\nxor rdi, rdi\nsyscall\n";
    return asmCode.str();
}

// === Compressor ===
int compressData(const std::string& input, std::string& output) {
    uLongf size = compressBound(input.size());
    std::vector<char> buf(size);
    if (compress((Bytef*)buf.data(), &size, (const Bytef*)input.data(), input.size()) != Z_OK) return -1;
    output.assign(buf.data(), size);
    return 0;
}

// === Capsule Writer ===
void writeCapsule(const std::string& asmCode, const std::string& src) {
    std::string zasm, zsrc;
    compressData(asmCode, zasm);
    compressData(src, zsrc);
    std::ofstream out("out.qtrcapsule", std::ios::binary);
    out.write("QTRC", 4);
    uint32_t version = 1;
    out.write((char*)&version, 4);
    uint32_t asz = zasm.size(), ssz = zsrc.size();
    out.write((char*)&asz, 4); out.write((char*)&ssz, 4);
    out.write(zasm.data(), asz); out.write(zsrc.data(), ssz);
    out.close();
    std::cout << "[Quarter] Capsule written.\n";
}

// === QuarterGPT-Local via LLM ===
std::string queryQuarterGPT(const std::string& prompt) {
    FILE* pipe = popen(("llama-query \"" + prompt + "\"").c_str(), "r");
    if (!pipe) return "ERROR";
    char buffer[512];
    std::string result;
    while (fgets(buffer, 512, pipe)) result += buffer;
    pclose(pipe);
    return result;
}

// === Visualizer ===
void visualizeAST(const std::unique_ptr<ASTNode>& node, int indent = 0) {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << node->type << ": " << node->value << "\n";
    for (const auto& child : node->children) {
        visualizeAST(child, indent + 1);
    }
}

// === Main Compiler ===
int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: quarter <file.qtr>\n";
        return 1;
    }

    std::ifstream in(argv[1]);
    std::stringstream buffer; buffer << in.rdbuf();
    std::string source = buffer.str();

    auto tokens = tokenize(source);
    auto ast = parse(tokens);
    auto asmCode = emitASM(ast);

    std::ofstream asmOut("out.asm"); asmOut << asmCode;
    system("nasm -f elf64 out.asm -o out.o");
    system("ld out.o -o out");

    writeCapsule(asmCode, source);
    visualizeAST(ast);

    std::string gptResponse = queryQuarterGPT("Optimize this: " + source);
    std::cout << "[QuarterGPT Response]\n" << gptResponse << "\n";

    return 0;
}
#include <iostream>
#include <fstream>
#include <regex>
#include <string>

void highlightQuarterCode(const std::string& code) {
    std::regex keyword(R"(\b(star|end|val|var|loop|from|to|say|define|procedure|nest|dg)\b)");
    std::regex string_literal(R"("(.*?)")");
    std::regex number(R"(\b\d+\b)");
    std::regex comment(R"(//.*?$)");

    std::string output = code;

    // Apply string highlight
    output = std::regex_replace(output, string_literal, "\033[32m\"$1\"\033[0m");

    // Apply number highlight
    output = std::regex_replace(output, number, "\033[36m$&\033[0m");

    // Apply keyword highlight
    output = std::regex_replace(output, keyword, "\033[35m$&\033[0m");

    // Apply comment highlight
    output = std::regex_replace(output, comment, "\033[90m$&\033[0m");

    std::cout << output << std::endl;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: qtrhighlighter <file.qtr>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Could not open file.\n";
        return 1;
    }

    std::string code((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    highlightQuarterCode(code);
    return 0;
}

#include <cstdlib>
#include <filesystem>
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: qtrrun <file.qtr>\n";
        return 1;
    }

    std::string filename = argv[1];
    std::string cmd = "quarterc " + filename + " && ./out";

    std::cout << "[Quarter] Running: " << filename << "\n";
    int result = std::system(cmd.c_str());

    if (result != 0) {
        std::cerr << "[Quarter] Execution failed with code: " << result << "\n";
    }

    return result;
}
// ================= QuarterLang Dominant Compiler =================
// Version: QTR-ProX 1.0
// Features: Modular, Legacy-Compatible, Extensible, NASM-linked, Capsule-Encoded
// =================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <regex>
#include <zlib.h>

// --------------------- Tokenizer ---------------------
enum TokenType {
    IDENTIFIER, KEYWORD, NUMBER, STRING, SYMBOL, NEWLINE, END
};

struct Token {
    TokenType type;
    std::string text;
    int line;
};

const std::unordered_set<std::string> keywords = {
    "star", "end", "val", "var", "loop", "from", "to", "say", "match", "pipe", "dg", "capsule"
};

std::vector<Token> tokenize(const std::string& code) {
    std::vector<Token> tokens;
    std::regex tokenPattern(R"((\".*?\")|([a-zA-Z_][a-zA-Z0-9_]*)|(\d+)|([=+\-*/(){};])|(\n))");
    std::smatch match;
    std::string::const_iterator searchStart(code.cbegin());
    int line = 1;

    while (std::regex_search(searchStart, code.cend(), match, tokenPattern)) {
        if (match[1].matched)
            tokens.push_back({ STRING, match[1], line });
        else if (match[2].matched) {
            auto word = match[2].str();
            tokens.push_back({ keywords.count(word) ? KEYWORD : IDENTIFIER, word, line });
        }
        else if (match[3].matched)
            tokens.push_back({ NUMBER, match[3], line });
        else if (match[4].matched)
            tokens.push_back({ SYMBOL, match[4], line });
        else if (match[5].matched)
            line++;

        searchStart = match.suffix().first;
    }

    tokens.push_back({ END, "", line });
    return tokens;
}

// --------------------- AST ---------------------
struct ASTNode {
    std::string type;
    std::string value;
    std::vector<ASTNode*> children;
};

ASTNode* parse(const std::vector<Token>& tokens) {
    size_t index = 0;
    auto peek = [&]() -> const Token& { return tokens[index]; };
    auto advance = [&]() -> const Token& { return tokens[index++]; };

    auto parseStatement = [&]() -> ASTNode* {
        if (peek().text == "say") {
            advance();
            ASTNode* node = new ASTNode{ "say", advance().text };
            return node;
        }
        else if (peek().text == "val") {
            advance();
            std::string name = advance().text;
            advance(); // =
            std::string value = advance().text;
            ASTNode* node = new ASTNode{ "val", name };
            node->children.push_back(new ASTNode{ "value", value });
            return node;
        }
        return nullptr;
    };

    ASTNode* root = new ASTNode{ "program", "" };
    while (peek().type != END) {
        if (auto stmt = parseStatement()) root->children.push_back(stmt);
        else advance();
    }
    return root;
}

// --------------------- NASM Emitter ---------------------
std::string emitASM(ASTNode* root) {
    std::stringstream asmCode;

    asmCode << "section .data\n";
    for (auto* node : root->children) {
        if (node->type == "say") {
            asmCode << "    msg db " << node->value << ", 10\n";
            asmCode << "    msg_len equ $ - msg\n";
        }
    }

    asmCode << "\nsection .text\n";
    asmCode << "    global _start\n";
    asmCode << "_start:\n";
    asmCode << "    mov rax, 1\n";
    asmCode << "    mov rdi, 1\n";
    asmCode << "    mov rsi, msg\n";
    asmCode << "    mov rdx, msg_len\n";
    asmCode << "    syscall\n";
    asmCode << "    mov rax, 60\n";
    asmCode << "    xor rdi, rdi\n";
    asmCode << "    syscall\n";

    return asmCode.str();
}

// --------------------- Capsule Compressor ---------------------
std::string compressData(const std::string& input) {
    uLong srcLen = input.size();
    uLong destLen = compressBound(srcLen);
    std::string output(destLen, 0);
    if (compress((Bytef*)&output[0], &destLen, (const Bytef*)input.c_str(), srcLen) != Z_OK)
        return "";
    output.resize(destLen);
    return output;
}

// --------------------- Main Compiler Entry ---------------------
int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: quarterc <file.qtr>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Could not open source file.\n";
        return 1;
    }

    std::string code((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    auto tokens = tokenize(code);
    ASTNode* ast = parse(tokens);
    std::string asmCode = emitASM(ast);

    // Save .asm file
    std::ofstream out("out.asm");
    out << asmCode;
    out.close();

    // Assemble and link
    std::system("nasm -f elf64 out.asm -o out.o");
    std::system("ld out.o -o out");

    // Save .qtrcapsule binary
    std::string capsule = compressData(code + "\n----\n" + asmCode);
    std::ofstream capfile("program.qtrcapsule", std::ios::binary);
    capfile.write(capsule.data(), capsule.size());
    capfile.close();

    std::cout << "[QuarterLang] Compilation complete.\n";
    return 0;
}
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <memory>

// Adjust based on llama.cpp location and bindings
#define LLAMA_MODEL_PATH "./models/ggml-model-q4_0.bin"
#define LLAMA_CLI "./llama/main"

std::string queryQuarterGPT(const std::string& prompt) {
    std::ofstream promptFile("q_prompt.txt");
    promptFile << prompt;
    promptFile.close();

    std::string command = std::string(LLAMA_CLI) + " -m " + LLAMA_MODEL_PATH +
                          " -p q_prompt.txt -n 256 --silent-prompt > q_response.txt";
    std::system(command.c_str());

    std::ifstream responseFile("q_response.txt");
    std::stringstream result;
    result << responseFile.rdbuf();

    return result.str();
}

std::string userAsk;
std::cout << "Ask QuarterGPT-Local for help or hints (type & press enter): ";
std::getline(std::cin, userAsk);

if (!userAsk.empty()) {
    std::string advice = queryQuarterGPT("Help me write this QuarterLang code:\n" + userAsk);
    std::cout << "\n🧠 QuarterGPT says:\n" << advice << std::endl;
}

std::ofstream mapfile("program.qtrcapsule.map");
mapfile << "[Capsule Map Metadata]\\n";
mapfile << "Tokens: " << tokens.size() << "\\n";
mapfile << "AST Nodes: " << ast->children.size() << "\\n";
mapfile << "Compressed Capsule Size: " << capsule.size() << " bytes\\n";
mapfile << "\\n-- AST Structure --\\n";
for (const auto* child : ast->children)
    mapfile << "Node: " << child->type << " -> " << child->value << "\\n";
mapfile.close();
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <zlib.h>
#include <cstdlib>

bool decompressCapsule(const std::string& filename, std::string& outSource, std::string& outASM) {
    std::ifstream infile(filename, std::ios::binary | std::ios::ate);
    if (!infile.is_open()) return false;

    std::streamsize size = infile.tellg();
    infile.seekg(0, std::ios::beg);
    std::vector<char> compressed(size);
    if (!infile.read(compressed.data(), size)) return false;

    uLongf decompressedSize = 100000;  // Adjust for large capsules
    std::vector<char> decompressed(decompressedSize);
    if (uncompress((Bytef*)decompressed.data(), &decompressedSize, (Bytef*)compressed.data(), size) != Z_OK)
        return false;

    std::string fullData(decompressed.begin(), decompressed.begin() + decompressedSize);
    auto sep = fullData.find("\n----\n");
    if (sep == std::string::npos) return false;

    outSource = fullData.substr(0, sep);
    outASM = fullData.substr(sep + 6);
    return true;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: qtrrun <file.qtrcapsule>\n";
        return 1;
    }

    std::string src, asmCode;
    if (!decompressCapsule(argv[1], src, asmCode)) {
        std::cerr << "Failed to read capsule.\n";
        return 1;
    }

    std::ofstream fasm("runtime_out.asm");
    fasm << asmCode;
    fasm.close();

    std::system("nasm -f elf64 runtime_out.asm -o runtime_out.o");
    std::system("ld runtime_out.o -o runtime_exec");
    std::system("./runtime_exec");

    return 0;
}
std::vector<std::string> suggestCorrections(const std::string& token) {
    static const std::vector<std::string> keywords = {
        "star", "end", "val", "var", "say", "loop", "from", "to",
        "when", "nest", "dg", "procedure", "return", "define"
    };

    std::vector<std::string> suggestions;
    for (const auto& kw : keywords) {
        int dist = levenshtein(token, kw);
        if (dist <= 2) suggestions.push_back(kw);
    }
    return suggestions;
}

std::string optimizeASM(const std::string& raw) {
    std::stringstream input(raw);
    std::stringstream output;
    std::string line;

    while (std::getline(input, line)) {
        if (line.find("mov rax, 0") != std::string::npos) {
            output << "xor rax, rax\n";  // faster, smaller
        } else if (line.find("add rax, 1") != std::string::npos) {
            output << "inc rax\n"; // use inc for speed
        } else {
            output << line << "\n";
        }
    }

    return output.str();
}

std::vector<uint8_t> packCapsuleBinary(const std::string& source, const std::string& asmCode) {
    std::string combined = source + "\n----\n" + asmCode;
    uLongf compressedSize = compressBound(combined.size());
    std::vector<uint8_t> compressed(compressedSize);

    compress(compressed.data(), &compressedSize, (Bytef*)combined.data(), combined.size());
    compressed.resize(compressedSize);
    return compressed;
}

void foldConstants(ASTNode* node) {
    for (auto* child : node->children) foldConstants(child);

    if (node->type == "add" &&
        node->left->type == "number" &&
        node->right->type == "number") {
        int a = std::stoi(node->left->value);
        int b = std::stoi(node->right->value);
        node->type = "number";
        node->value = std::to_string(a + b);
        node->left = nullptr;
        node->right = nullptr;
    }
}

struct Symbol {
    std::string name;
    std::string type;
    int scopeLevel;
};

class SymbolTable {
    std::vector<std::unordered_map<std::string, Symbol>> scopes;

public:
    void enterScope() { scopes.push_back({}); }

    void exitScope() {
        if (!scopes.empty()) scopes.pop_back();
    }

    void declare(const std::string& name, const std::string& type) {
        if (!scopes.empty()) scopes.back()[name] = { name, type, (int)scopes.size() - 1 };
    }

    std::optional<Symbol> resolve(const std::string& name) {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            if (it->count(name)) return (*it)[name];
        }
        return std::nullopt;
    }
};

#include <thread>

void compileInParallel(const std::vector<std::string>& files) {
    std::vector<std::thread> threads;

    for (const auto& file : files) {
        threads.emplace_back([=]() {
            compileQuarterFile(file);  // your existing compile fn
        });
    }

    for (auto& t : threads) t.join();
}

void generateCapsuleMapJson(ASTNode* root, const std::string& outputFile) {
    std::ofstream out(outputFile);
    out << "{\n  \"type\": \"" << root->type << "\",\n  \"children\": [\n";

    for (size_t i = 0; i < root->children.size(); ++i) {
        ASTNode* child = root->children[i];
        out << "    { \"type\": \"" << child->type << "\", \"value\": \"" << child->value << "\" }";
        if (i < root->children.size() - 1) out << ",";
        out << "\n";
    }

    out << "  ]\n}\n";
}

std::string callQuarterGPTLocal(const std::string& prompt) {
    std::ofstream f("prompt.txt");
    f << prompt;
    f.close();

    std::string command = "./llama -m ./model.ggml -p prompt.txt -n 100 --silent-prompt > response.txt";
    std::system(command.c_str());

    std::ifstream resp("response.txt");
    return std::string((std::istreambuf_iterator<char>(resp)), {});
}

struct Dependency {
    std::string file;
    std::vector<std::string> includes;
};

void buildDependencyGraph(const std::string& entry) {
    std::unordered_map<std::string, Dependency> graph;
    std::set<std::string> visited;

    std::function<void(const std::string&)> dfs = [&](const std::string& file) {
        if (visited.count(file)) return;
        visited.insert(file);

        Dependency dep;
        dep.file = file;

        std::ifstream f(file);
        std::string line;
        while (std::getline(f, line)) {
            if (line.rfind("use ", 0) == 0) {
                std::string inc = line.substr(4);
                dep.includes.push_back(inc);
                dfs(inc);
            }
        }

        graph[file] = dep;
    };

    dfs(entry);

    for (const auto& [file, dep] : graph) {
        std::cout << file << " includes:\n";
        for (const auto& inc : dep.includes)
            std::cout << "  - " << inc << "\n";
    }
}
#include <fstream>
#include <string>
#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>

std::string callQuarterGPTLocal(const std::string& prompt) {
    std::ofstream out("qtr_prompt.txt");
    out << prompt;
    out.close();

    std::string command = "./llama -m models/quarterggml.bin -p qtr_prompt.txt -n 80 > qtr_output.txt";
    std::system(command.c_str());

    std::ifstream in("qtr_output.txt");
    return std::string((std::istreambuf_iterator<char>(in)), {});
}

void watchFileAndSuggest(const std::string& filename) {
    std::string last = "";

    while (true) {
        std::ifstream file(filename);
        std::string code((std::istreambuf_iterator<char>(file)), {});
        if (code != last && !code.empty()) {
            last = code;
            std::cout << "\n🔧 [Quarter AI] Thinking...\n";
            std::string response = callQuarterGPTLocal("Complete or suggest fixes for:\n" + code);
            std::cout << "\n🧠 [AI Suggestion]:\n" << response << "\n";
        }
        std::this_thread::sleep_for(std::chrono::seconds(4));
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: quarter-ai <file.qtr>\n";
        return 1;
    }
    watchFileAndSuggest(argv[1]);
    return 0;
}

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <filesystem>
namespace fs = std::filesystem;

struct QtrPackage {
    std::string name;
    std::string version;
    std::vector<std::string> dependencies;
    std::string main;
};

QtrPackage parseQtrPkg(const std::string& path) {
    QtrPackage pkg;
    std::ifstream f(path);
    std::string line;

    while (std::getline(f, line)) {
        if (line.rfind("name:", 0) == 0) pkg.name = line.substr(5);
        else if (line.rfind("version:", 0) == 0) pkg.version = line.substr(8);
        else if (line.rfind("main:", 0) == 0) pkg.main = line.substr(5);
        else if (line.rfind("require:", 0) == 0)
            pkg.dependencies.push_back(line.substr(8));
    }

    return pkg;
}

void resolveDependencies(const QtrPackage& pkg) {
    std::cout << "📦 Resolving " << pkg.name << "...\n";
    for (const auto& dep : pkg.dependencies) {
        std::cout << "   - 📥 Pulling " << dep << "...\n";
        std::string depPath = "qtr_packages/" + dep + "/pkg.qtrpkg";
        if (fs::exists(depPath)) {
            QtrPackage d = parseQtrPkg(depPath);
            resolveDependencies(d);
        } else {
            std::cout << "❌ Missing dependency: " << dep << "\n";
        }
    }
}

void buildQtrPkg(const QtrPackage& pkg) {
    std::string mainFile = "src/" + pkg.main;
    std::string command = "quarterc " + mainFile + " -o out/" + pkg.name + ".qtrcapsule";
    std::cout << "\n🏗️ Building with: " << command << "\n";
    std::system(command.c_str());
}

int main() {
    QtrPackage root = parseQtrPkg("pkg.qtrpkg");
    resolveDependencies(root);
    buildQtrPkg(root);
    return 0;
}
#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <variant>
#include <vector>
#include <iostream>

using QtrArg = std::variant<int, float, std::string>;
using QtrArgs = std::vector<QtrArg>;
using QtrFn = std::function<QtrArg(QtrArgs)>;

class QuarterOpsEngine {
public:
    void bind(const std::string& name, QtrFn fn);
    QtrArg call(const std::string& name, const QtrArgs& args);
    void loadBuiltins();
    void exposeToScript(const std::string& filename); // Optional

private:
    std::unordered_map<std::string, QtrFn> ops;
};

#include "QuarterOpsEngine.hpp"
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <thread>
#include <chrono>

void QuarterOpsEngine::bind(const std::string& name, QtrFn fn) {
    ops[name] = fn;
}

QtrArg QuarterOpsEngine::call(const std::string& name, const QtrArgs& args) {
    if (ops.find(name) != ops.end()) {
        return ops[name](args);
    }
    throw std::runtime_error("Unknown operation: " + name);
}

void QuarterOpsEngine::loadBuiltins() {
    bind("print", [](QtrArgs args) -> QtrArg {
        for (auto& arg : args) {
            std::visit([](auto&& val) { std::cout << val << " "; }, arg);
        }
        std::cout << std::endl;
        return 0;
    });

    bind("read_file", [](QtrArgs args) -> QtrArg {
        std::ifstream f(std::get<std::string>(args[0]));
        return std::string((std::istreambuf_iterator<char>(f)), {});
    });

    bind("write_file", [](QtrArgs args) -> QtrArg {
        std::ofstream f(std::get<std::string>(args[0]));
        f << std::get<std::string>(args[1]);
        return 0;
    });

    bind("delay_ms", [](QtrArgs args) -> QtrArg {
        std::this_thread::sleep_for(std::chrono::milliseconds(std::get<int>(args[0])));
        return 0;
    });

    bind("system", [](QtrArgs args) -> QtrArg {
        return std::system(std::get<std::string>(args[0]).c_str());
    });

    bind("exists", [](QtrArgs args) -> QtrArg {
        return std::filesystem::exists(std::get<std::string>(args[0])) ? 1 : 0;
    });

    bind("math_add", [](QtrArgs args) -> QtrArg {
        return std::get<float>(args[0]) + std::get<float>(args[1]);
    });

    bind("to_string", [](QtrArgs args) -> QtrArg {
        return std::to_string(std::get<int>(args[0]));
    });

    bind("launch_tool", [](QtrArgs args) -> QtrArg {
        std::string tool = std::get<std::string>(args[0]);
        std::string param = std::get<std::string>(args[1]);
        std::string cmd = tool + " " + param;
        return std::system(cmd.c_str());
    });
}

#include "QuarterOpsEngine.hpp"

int main() {
    QuarterOpsEngine ops;
    ops.loadBuiltins();

    ops.call("print", { "QuarterLang ⚡️ Running on Real Ops!" });
    ops.call("write_file", { "hello.txt", "This is real world!" });

    QtrArg result = ops.call("read_file", { "hello.txt" });
    std::cout << "✅ Read: " << std::get<std::string>(result) << std::endl;

    ops.call("delay_ms", { 500 });
    ops.call("system", { "echo QuarterLang is versatile!" });

    return 0;
}
#pragma once
#include "QuarterOpsEngine.hpp"

class QuarterRuntimeEngine {
public:
    QuarterRuntimeEngine();
    void executeScript(const std::string& path);
    void bindOps(const std::string& name, QtrFn fn);
    QuarterOpsEngine ops;
};

#include "QuarterRuntimeEngine.hpp"
#include <fstream>
#include <iostream>

QuarterRuntimeEngine::QuarterRuntimeEngine() {
    ops.loadBuiltins();
}

void QuarterRuntimeEngine::bindOps(const std::string& name, QtrFn fn) {
    ops.bind(name, fn);
}

void QuarterRuntimeEngine::executeScript(const std::string& path) {
    std::ifstream f(path);
    std::string line;

    while (std::getline(f, line)) {
        if (line.find("say") == 0) {
            auto msg = line.substr(4);
            ops.call("print", { msg });
        }
        else if (line.find("wait") == 0) {
            int ms = std::stoi(line.substr(5));
            ops.call("delay_ms", { ms });
        }
        // Add more command mapping here
    }
}

#include "QuarterOpsEngine.hpp"
#include <SDL2/SDL.h>

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

void bindSDLToOps(QuarterOpsEngine& ops) {
    ops.bind("init_window", [](QtrArgs args) -> QtrArg {
        SDL_Init(SDL_INIT_VIDEO);
        window = SDL_CreateWindow("Quarter SDL Window", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        return 1;
    });

    ops.bind("clear_screen", [](QtrArgs args) -> QtrArg {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        return 0;
    });

    ops.bind("draw_rect", [](QtrArgs args) -> QtrArg {
        int x = std::get<int>(args[0]);
        int y = std::get<int>(args[1]);
        int w = std::get<int>(args[2]);
        int h = std::get<int>(args[3]);
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect r = { x, y, w, h };
        SDL_RenderFillRect(renderer, &r);
        return 0;
    });

    ops.bind("present", [](QtrArgs) -> QtrArg {
        SDL_RenderPresent(renderer);
        return 0;
    });

    ops.bind("delay", [](QtrArgs args) -> QtrArg {
        SDL_Delay(std::get<int>(args[0]));
        return 0;
    });

    ops.bind("close_window", [](QtrArgs) -> QtrArg {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    });
}

#include "QuarterOpsEngine.hpp"
#include "QuarterRuntimeEngine.hpp"
#include "QuarterSDLModule.cpp"

extern "C" {

    QuarterRuntimeEngine* create_engine() {
        auto* engine = new QuarterRuntimeEngine();
        bindSDLToOps(engine->ops);
        return engine;
    }

    void destroy_engine(QuarterRuntimeEngine* engine) {
        delete engine;
    }

    void run_script(QuarterRuntimeEngine* engine, const char* path) {
        engine->executeScript(std::string(path));
    }

    void inject_var(QuarterRuntimeEngine* engine, const char* key, int val) {
        engine->bindOps(key, [val](QtrArgs) -> QtrArg { return val; });
    }
}

#include "QuarterRuntimeEngine.hpp"
#include "QuarterSDLModule.cpp"

int main() {
    QuarterRuntimeEngine engine;
    bindSDLToOps(engine.ops);

    engine.executeScript("example.qtr");

    return 0;
}
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>

std::vector<SDL_Texture*> loadedFrames;
int currentFrame = 0;

void bindAudioAnimToOps(QuarterOpsEngine& ops, SDL_Renderer* renderer) {
    Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);

    ops.bind("play_sound", [](QtrArgs args) -> QtrArg {
        Mix_Chunk* sound = Mix_LoadWAV(std::get<std::string>(args[0]).c_str());
        if (sound) Mix_PlayChannel(-1, sound, 0);
        return 1;
    });

    ops.bind("load_frames", [renderer](QtrArgs args) -> QtrArg {
        std::string base = std::get<std::string>(args[0]);
        int count = std::get<int>(args[1]);
        for (int i = 0; i < count; ++i) {
            std::string path = base + std::to_string(i) + ".png";
            SDL_Surface* img = IMG_Load(path.c_str());
            if (img) {
                SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, img);
                loadedFrames.push_back(tex);
                SDL_FreeSurface(img);
            }
        }
        return 1;
    });

    ops.bind("show_frame", [renderer](QtrArgs args) -> QtrArg {
        int index = std::get<int>(args[0]);
        if (index < loadedFrames.size()) {
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, loadedFrames[index], NULL, NULL);
            SDL_RenderPresent(renderer);
        }
        return 0;
    });

    ops.bind("animate", [renderer](QtrArgs args) -> QtrArg {
        int delay = std::get<int>(args[0]);
        for (auto& frame : loadedFrames) {
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, frame, NULL, NULL);
            SDL_RenderPresent(renderer);
            SDL_Delay(delay);
        }
        return 0;
    });
}

#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

GLuint loadShader(const std::string& vertPath, const std::string& fragPath);
void drawGPUQuad(GLuint shader, float r, float g, float b);

void bindGPUCapsuleOps(QuarterOpsEngine& ops) {
    GLuint shader = loadShader("shader.vert", "shader.frag");

    ops.bind("gpu_draw_color", [shader](QtrArgs args) -> QtrArg {
        float r = std::get<float>(args[0]);
        float g = std::get<float>(args[1]);
        float b = std::get<float>(args[2]);
        drawGPUQuad(shader, r, g, b);
        return 1;
    });
}

GLuint loadShader(const std::string& vertPath, const std::string& fragPath) {
    std::ifstream vfile(vertPath), ffile(fragPath);
    std::string vsrc((std::istreambuf_iterator<char>(vfile)), {});
    std::string fsrc((std::istreambuf_iterator<char>(ffile)), {});

    const char* vptr = vsrc.c_str();
    const char* fptr = fsrc.c_str();

    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vert, 1, &vptr, NULL);
    glShaderSource(frag, 1, &fptr, NULL);
    glCompileShader(vert);
    glCompileShader(frag);

    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    return program;
}

void drawGPUQuad(GLuint shader, float r, float g, float b) {
    float verts[] = { -0.5f, -0.5f, 0.0f,
                       0.5f, -0.5f, 0.0f,
                       0.0f,  0.5f, 0.0f };
    GLuint VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glUseProgram(shader);
    GLint loc = glGetUniformLocation(shader, "color");
    glUniform3f(loc, r, g, b);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
void bindInstancedDraw(QuarterOpsEngine& ops) {
    static GLuint VAO, VBO, instanceVBO;
    static GLuint shader = loadShader("instanced.vert", "simple.frag");

    float quad[] = {
        -0.05f, -0.05f,
         0.05f, -0.05f,
        -0.05f,  0.05f,
         0.05f,  0.05f,
    };

    std::vector<glm::vec2> offsets;

    ops.bind("gpu_instance_reset", [&](QtrArgs) -> QtrArg {
        offsets.clear();
        return 1;
    });

    ops.bind("gpu_instance_add", [&](QtrArgs args) -> QtrArg {
        float x = std::get<float>(args[0]);
        float y = std::get<float>(args[1]);
        offsets.emplace_back(x, y);
        return 1;
    });

    ops.bind("gpu_instance_draw", [&](QtrArgs args) -> QtrArg {
        if (offsets.empty()) return 0;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &instanceVBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, offsets.size() * sizeof(glm::vec2), &offsets[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribDivisor(1, 1); // key: advance every instance

        glUseProgram(shader);
        glUniform3f(glGetUniformLocation(shader, "color"), 0.0f, 0.6f, 1.0f);
        glBindVertexArray(VAO);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, offsets.size());

        return offsets.size();
    });
}

#include <cstdlib>
#include <fstream>

std::string callQuarterGPT(const std::string& prompt) {
    std::ofstream out("prompt.txt");
    out << prompt;
    out.close();

    system("./qgpt-local prompt.txt > response.txt");

    std::ifstream in("response.txt");
    std::string line, result;
    while (std::getline(in, line)) result += line + "\n";
    return result;
}

ops.bind("ai_vision", [](QtrArgs args) -> QtrArg {
    std::string capsuleContext = std::get<std::string>(args[0]);
    std::string result = callQuarterGPT("What should be visualized in scene: " + capsuleContext);
    std::cout << "[AI Suggestion]: " << result << std::endl;
    return 1;
});
#include <SDL2/SDL.h>
#include <map>

std::map<std::string, bool> controllerState;
SDL_GameController* gameController = nullptr;

void initController() {
    if (SDL_NumJoysticks() > 0) {
        if (SDL_IsGameController(0)) {
            gameController = SDL_GameControllerOpen(0);
        }
    }
}

void updateControllerState() {
    SDL_GameControllerUpdate();
    controllerState["a"] = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_A);
    controllerState["b"] = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_B);
    controllerState["x"] = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_X);
    controllerState["y"] = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_Y);
    controllerState["start"] = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_START);
    controllerState["back"] = SDL_GameControllerGetButton(gameController, SDL_CONTROLLER_BUTTON_BACK);
}

void bindControllerHooks(QuarterOpsEngine& ops) {
    initController();

    ops.bind("controller_poll", [](QtrArgs) -> QtrArg {
        updateControllerState();
        return 1;
    });

    ops.bind("controller_button", [](QtrArgs args) -> QtrArg {
        std::string button = std::get<std::string>(args[0]);
        return controllerState[button] ? 1 : 0;
    });
}

std::string autoCompleteFromCapsule(const std::string& capsuleCode) {
    std::ofstream out("vision_prompt.qtr");
    out << "Auto-complete visually:\n" << capsuleCode;
    out.close();
    system("./qgpt-local vision_prompt.qtr > vision_response.qtr");

    std::ifstream in("vision_response.qtr");
    std::stringstream result;
    result << in.rdbuf();
    return result.str();
}

void bindAutoCompleteAI(QuarterOpsEngine& ops) {
    ops.bind("ai_autocomplete", [](QtrArgs args) -> QtrArg {
        std::string current = std::get<std::string>(args[0]);
        std::string completed = autoCompleteFromCapsule(current);
        std::cout << "[AI Completed Code]:\n" << completed << std::endl;
        return 1;
    });
}

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include "QuarterCapsuleRunner.hpp"

void renderFrame() {
    glClear(GL_COLOR_BUFFER_BIT);
    runCapsuleTick();  // from QuarterCapsuleRunner
    eglSwapBuffers(display, surface);
}
#pragma once
#include <string>
#include <map>
#include <vector>

struct QtrPackage {
    std::string name;
    std::string version;
    std::string entrypoint;
    std::vector<std::string> dependencies;
    std::map<std::string, std::string> files;
};

class QuarterPackageManager {
public:
    bool loadPackage(const std::string& path, QtrPackage& out);
    bool resolveDependencies(const QtrPackage& pkg);
    void buildCapsule(const QtrPackage& pkg);
};

#include "QuarterPackageManager.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <filesystem>
using json = nlohmann::json;

bool QuarterPackageManager::loadPackage(const std::string& path, QtrPackage& out) {
    std::ifstream f(path + "/qtrpkg.json");
    json j;
    f >> j;
    out.name = j["name"];
    out.version = j["version"];
    out.entrypoint = j["entrypoint"];
    for (const auto& dep : j["dependencies"]) out.dependencies.push_back(dep);
    for (const auto& f : std::filesystem::directory_iterator(path + "/src"))
        out.files[f.path().filename().string()] = f.path().string();
    return true;
}

bool QuarterPackageManager::resolveDependencies(const QtrPackage& pkg) {
    for (const std::string& dep : pkg.dependencies) {
        std::string depPath = "libs/" + dep;
        if (!std::filesystem::exists(depPath)) {
            std::cerr << "Missing dependency: " << dep << std::endl;
            return false;
        }
    }
    return true;
}

void QuarterPackageManager::buildCapsule(const QtrPackage& pkg) {
    std::ofstream capsule(pkg.name + ".qtrcapsule");
    for (const auto& [fname, fpath] : pkg.files) {
        std::ifstream code(fpath);
        capsule << "// FILE: " << fname << "\n";
        capsule << code.rdbuf() << "\n\n";
    }
}

#pragma once
#include <string>
#include <map>
#include <functional>

class QuarterMultiplayer {
public:
    void connectWebSocket(const std::string& url);
    void startLocalPeer(const std::string& name, int port);
    void broadcast(const std::string& event, const std::string& payload);
    void onReceive(std::function<void(const std::string&, const std::string&)> handler);
private:
    std::function<void(const std::string&, const std::string&)> receiveHandler;
};

#include "QuarterMultiplayer.hpp"
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <thread>
#include <boost/asio.hpp>

typedef websocketpp::client<websocketpp::config::asio_client> client;
client wsClient;
websocketpp::connection_hdl wsHandle;

void QuarterMultiplayer::connectWebSocket(const std::string& url) {
    wsClient.init_asio();
    wsClient.set_message_handler([this](websocketpp::connection_hdl, client::message_ptr msg) {
        auto payload = msg->get_payload();
        auto split = payload.find(':');
        receiveHandler(payload.substr(0, split), payload.substr(split + 1));
    });

    auto con = wsClient.get_connection(url, websocketpp::lib::error_code());
    wsHandle = con->get_handle();
    wsClient.connect(con);
    std::thread([&](){ wsClient.run(); }).detach();
}

void QuarterMultiplayer::startLocalPeer(const std::string& name, int port) {
    std::thread([this, port]() {
        using boost::asio::ip::udp;
        boost::asio::io_service io;
        udp::socket socket(io, udp::endpoint(udp::v4(), port));
        char data[1024];
        udp::endpoint sender;

        while (true) {
            size_t len = socket.receive_from(boost::asio::buffer(data), sender);
            std::string msg(data, len);
            auto split = msg.find(':');
            receiveHandler(msg.substr(0, split), msg.substr(split + 1));
        }
    }).detach();
}

void QuarterMultiplayer::broadcast(const std::string& event, const std::string& payload) {
    std::string full = event + ":" + payload;
    wsClient.send(wsHandle, full, websocketpp::frame::opcode::text);
}

void QuarterMultiplayer::onReceive(std::function<void(const std::string&, const std::string&)> handler) {
    receiveHandler = handler;
}

#pragma once
#include <string>
#include <map>
#include <vector>

struct Bone {
    std::string name;
    float angle;
    float x, y;
};

struct SpriteAttachment {
    std::string image;
    float offsetX, offsetY, scaleX, scaleY;
};

struct AnimatedCharacter {
    std::map<std::string, Bone> bones;
    std::map<std::string, SpriteAttachment> attachments;
};

class QuarterAnimationEngine {
public:
    void loadRig(const std::string& rigFile);
    void updateFrame(float dt);
    void render();
    AnimatedCharacter character;
};

#include "QuarterAnimationEngine.hpp"
#include <nlohmann/json.hpp>
#include <stb_image.h>
#include <GL/glew.h>

void QuarterAnimationEngine::loadRig(const std::string& rigFile) {
    std::ifstream in(rigFile);
    nlohmann::json j;
    in >> j;
    for (auto& bone : j["bones"]) {
        Bone b;
        b.name = bone["name"];
        b.x = bone["x"];
        b.y = bone["y"];
        b.angle = bone["angle"];
        character.bones[b.name] = b;
    }
    for (auto& att : j["attachments"]) {
        SpriteAttachment a;
        a.image = att["image"];
        a.offsetX = att["offsetX"];
        a.offsetY = att["offsetY"];
        a.scaleX = att["scaleX"];
        a.scaleY = att["scaleY"];
        character.attachments[att["bone"]] = a;
    }
}

void QuarterAnimationEngine::updateFrame(float dt) {
    for (auto& [name, bone] : character.bones)
        bone.angle += dt * 10.0f;
}

void QuarterAnimationEngine::render() {
    for (const auto& [bone, sprite] : character.attachments) {
        const Bone& b = character.bones[bone];
        drawSprite(sprite.image, b.x + sprite.offsetX, b.y + sprite.offsetY, sprite.scaleX, sprite.scaleY, b.angle);
    }
}
#pragma once
#include "QuarterAnimationEngine.hpp"

class QuarterAnimControllerBridge {
public:
    static void bind(QuarterAnimationEngine& engine);
};

#include "QuarterAnimControllerBridge.hpp"
#include "QuarterOpsController.cpp" // Uses SDL2 gamepad logic

void QuarterAnimControllerBridge::bind(QuarterAnimationEngine& engine) {
    initController();

    engine.character.bones["hip"].x = 0;
    engine.character.bones["hip"].y = 0;

    engine.character.bones["arm_left"].angle = 0;
    engine.character.bones["arm_right"].angle = 0;

    engine.character.bones["leg_left"].angle = 0;
    engine.character.bones["leg_right"].angle = 0;

    std::thread([&engine]() {
        while (true) {
            updateControllerState();

            if (controllerState["left"])
                engine.character.bones["hip"].x -= 1;
            if (controllerState["right"])
                engine.character.bones["hip"].x += 1;

            if (controllerState["a"]) {
                engine.character.bones["leg_left"].angle = -25;
                engine.character.bones["leg_right"].angle = 25;
            } else {
                engine.character.bones["leg_left"].angle = 0;
                engine.character.bones["leg_right"].angle = 0;
            }

            if (controllerState["b"]) {
                engine.character.bones["arm_left"].angle += 5;
                engine.character.bones["arm_right"].angle -= 5;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }).detach();
}

#include "QuarterAnimationEngine.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>

void generateBoneMotionFromAI(QuarterAnimationEngine& engine) {
    std::ofstream out("bone_prompt.qtr");
    out << "generate realistic bone animation movement for human sprint cycle:\n";

    for (auto& [name, bone] : engine.character.bones) {
        out << name << " " << bone.angle << "\n";
    }

    out.close();
    system("./qgpt-local bone_prompt.qtr > bone_output.qtr");

    std::ifstream in("bone_output.qtr");
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream ss(line);
        std::string boneName;
        float angle;
        ss >> boneName >> angle;
        engine.character.bones[boneName].angle = angle;
    }
}

void hookAIAnimationLoop(QuarterAnimationEngine& engine) {
    std::thread([&engine]() {
        while (true) {
            generateBoneMotionFromAI(engine);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }).detach();
}

QuarterAnimationEngine animEngine;
animEngine.loadRig("assets/hero.rig.json");

QuarterAnimControllerBridge::bind(animEngine);         // 🎮 Real controller hooks
hookAIAnimationLoop(animEngine);                       // 🧠 AI-driven bone motion

while (appRunning) {
    animEngine.updateFrame(0.016f);
    animEngine.render();
}
#pragma once
#include "QuarterAnimationEngine.hpp"

class MocapCapsuleImporter {
public:
    static void import(const std::string& mocapCSV, QuarterAnimationEngine& engine);
};

#include "MocapCapsuleImporter.hpp"
#include <fstream>
#include <sstream>
#include <map>

void MocapCapsuleImporter::import(const std::string& mocapCSV, QuarterAnimationEngine& engine) {
    std::ifstream in(mocapCSV);
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream ss(line);
        std::string bone;
        float angle;
        ss >> bone >> angle;
        if (engine.character.bones.count(bone)) {
            engine.character.bones[bone].angle = angle;
        }
    }
}

#include "QuarterAnimationEngine.hpp"
#include <fstream>

void exportToQtrMotion(const QuarterAnimationEngine& engine, const std::string& name) {
    std::ofstream out("motions/" + name + ".qtrmotion");
    for (const auto& [bone, data] : engine.character.bones) {
        out << bone << " " << data.angle << " " << data.x << " " << data.y << "\n";
    }
}

void applyQtrMotion(QuarterAnimationEngine& engine, const std::string& path) {
    std::ifstream in(path);
    std::string bone;
    float angle, x, y;
    while (in >> bone >> angle >> x >> y) {
        if (engine.character.bones.count(bone)) {
            engine.character.bones[bone].angle = angle;
            engine.character.bones[bone].x = x;
            engine.character.bones[bone].y = y;
        }
    }
}

#pragma once
#include "QuarterAnimationEngine.hpp"
#include "QuarterMultiplayer.hpp"

class AnimationNetSync {
public:
    static void syncLocalToNet(const QuarterAnimationEngine& engine, QuarterMultiplayer& net);
    static void syncNetToLocal(QuarterAnimationEngine& engine, const std::string& data);
};

#include "AnimationNetSync.hpp"
#include <sstream>

void AnimationNetSync::syncLocalToNet(const QuarterAnimationEngine& engine, QuarterMultiplayer& net) {
    std::ostringstream out;
    for (const auto& [name, bone] : engine.character.bones) {
        out << name << ":" << bone.angle << "," << bone.x << "," << bone.y << ";";
    }
    net.broadcast("anim_sync", out.str());
}

void AnimationNetSync::syncNetToLocal(QuarterAnimationEngine& engine, const std::string& data) {
    std::istringstream ss(data);
    std::string token;
    while (std::getline(ss, token, ';')) {
        auto colon = token.find(':');
        auto comma1 = token.find(',', colon);
        auto comma2 = token.find(',', comma1 + 1);

        std::string name = token.substr(0, colon);
        float angle = std::stof(token.substr(colon + 1, comma1 - colon - 1));
        float x = std::stof(token.substr(comma1 + 1, comma2 - comma1 - 1));
        float y = std::stof(token.substr(comma2 + 1));

        if (engine.character.bones.count(name)) {
            engine.character.bones[name].angle = angle;
            engine.character.bones[name].x = x;
            engine.character.bones[name].y = y;
        }
    }
}

// ⏳ Animate & send state
animEngine.updateFrame(dt);
AnimationNetSync::syncLocalToNet(animEngine, multiplayer);

// 🖼️ Draw
animEngine.render();

multiplayer.onReceive([](const std::string& event, const std::string& payload) {
    if (event == "anim_sync") {
        AnimationNetSync::syncNetToLocal(animEngine, payload);
    }
});
#include <fstream>
#include <chrono>
#include "QuarterAnimationEngine.hpp"

std::ofstream replayOut("replays/live.qtrreplay");
auto replayStart = std::chrono::steady_clock::now();

void recordReplayFrame(const QuarterAnimationEngine& engine) {
    auto now = std::chrono::steady_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now - replayStart).count();

    replayOut << timestamp;
    for (const auto& [bone, data] : engine.character.bones) {
        replayOut << " " << bone << ":" << data.angle << "," << data.x << "," << data.y;
    }
    replayOut << "\n";
}

#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include "QuarterAnimationEngine.hpp"

void playReplay(const std::string& path, QuarterAnimationEngine& engine) {
    std::ifstream in(path);
    std::string line;
    long lastTime = 0;

    while (std::getline(in, line)) {
        std::istringstream ss(line);
        long timestamp;
        ss >> timestamp;

        std::this_thread::sleep_for(std::chrono::milliseconds(timestamp - lastTime));
        lastTime = timestamp;

        std::string boneData;
        while (ss >> boneData) {
            auto colon = boneData.find(':');
            auto comma1 = boneData.find(',', colon);
            auto comma2 = boneData.find(',', comma1 + 1);

            std::string name = boneData.substr(0, colon);
            float angle = std::stof(boneData.substr(colon + 1, comma1 - colon - 1));
            float x = std::stof(boneData.substr(comma1 + 1, comma2 - comma1 - 1));
            float y = std::stof(boneData.substr(comma2 + 1));

            engine.character.bones[name] = { x, y, angle };
        }

        engine.render();
    }
}

#include <fstream>
#include <zlib.h>  // Requires zlib installed

void compressQtrMotion(const std::string& input, const std::string& output) {
    std::ifstream in(input, std::ios::binary);
    std::ofstream out(output, std::ios::binary);
    
    std::string data((std::istreambuf_iterator<char>(in)), {});
    uLongf compressedSize = compressBound(data.size());
    std::vector<char> compressed(compressedSize);

    compress((Bytef*)compressed.data(), &compressedSize, (const Bytef*)data.data(), data.size());

    out.write((char*)compressed.data(), compressedSize);
}

for (auto& [name, bone] : engine.character.bones) {
    ImGui::Text("%s", name.c_str());
    ImGui::SliderFloat(("Angle##" + name).c_str(), &bone.angle, -180.0f, 180.0f);
    ImGui::SliderFloat(("X##" + name).c_str(), &bone.x, -10.0f, 10.0f);
    ImGui::SliderFloat(("Y##" + name).c_str(), &bone.y, -10.0f, 10.0f);
}

#include <websocketpp/server.hpp>
#include "QuarterAnimationEngine.hpp"

QuarterAnimationEngine* g_engine = nullptr;

void onMocapData(const std::string& jsonData) {
    // Assume: { "hip": [0, 0, 10], "arm_left": [0, 0, 45], ... }
    auto data = parseJSON(jsonData); // use nlohmann/json or similar
    for (const auto& [bone, pose] : data.items()) {
        if (g_engine->character.bones.count(bone)) {
            g_engine->character.bones[bone].x = pose[0];
            g_engine->character.bones[bone].y = pose[1];
            g_engine->character.bones[bone].angle = pose[2];
        }
    }
}
#include <fstream>
#include <vector>
#include <map>

using ReplaySequence = std::vector<std::map<std::string, float>>;

ReplaySequence loadReplaySequence(const std::string& path) {
    ReplaySequence sequence;
    std::ifstream file(path);
    std::string line;

    while (std::getline(file, line)) {
        std::map<std::string, float> frame;
        std::istringstream ss(line);
        std::string token;
        ss >> token;  // skip timestamp

        while (ss >> token) {
            auto colon = token.find(':');
            auto comma1 = token.find(',', colon);
            auto name = token.substr(0, colon);
            float angle = std::stof(token.substr(colon + 1, comma1 - colon - 1));
            frame[name] = angle;
        }
        sequence.push_back(frame);
    }
    return sequence;
}

#include <fstream>
#include <vector>

void packageGameWithMotions(const std::string& outPath, const std::vector<std::string>& motions) {
    std::ofstream out(outPath, std::ios::binary);

    for (const auto& motionPath : motions) {
        std::ifstream in(motionPath, std::ios::binary);
        std::string data((std::istreambuf_iterator<char>(in)), {});
        uint32_t len = data.size();
        out.write((char*)&len, sizeof(len));
        out.write(data.c_str(), len);
    }

    out.close();
}

static int currentFrame = 0;
ImGui::SliderInt("Frame", &currentFrame, 0, replay.size() - 1);

if (ImGui::Button("<<")) currentFrame = std::max(0, currentFrame - 10);
ImGui::SameLine();
if (ImGui::Button(">>")) currentFrame = std::min((int)replay.size() - 1, currentFrame + 10);

static int trimStart = 0, trimEnd = replay.size() - 1;
ImGui::SliderInt("Trim Start", &trimStart, 0, replay.size() - 1);
ImGui::SliderInt("Trim End", &trimEnd, 0, replay.size() - 1);

if (ImGui::Button("Export Trimmed")) {
    std::ofstream out("trimmed.qtrreplay");
    for (int i = trimStart; i <= trimEnd; ++i) {
        out << serializeFrame(replay[i]) << "\n";
    }
}
// Capsules now self-optimize based on context and usage
class CapsuleIntelligence {
public:
    void observe(const std::string& func, float execTime);
    void rerouteOptimizedPath(); // JIT-toggle vs AOT split
    void autoCacheResults();     // Lock outputs for next session
};

struct CoupledCapsule {
    std::string motionFile;
    std::string soundEffect;
    std::string aiEmotionState;
    void executeSimultaneously();  // SDL2 + OpenAL + Capsule runner
};

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

std::map<int, websocketpp::connection_hdl> clients;
std::mutex connLock;

void broadcastMotion(const std::string& jsonMotion) {
    std::lock_guard<std::mutex> lock(connLock);
    for (auto& [id, conn] : clients)
        server.send(conn, jsonMotion, websocketpp::frame::opcode::text);
}

void onReceiveMotion(const std::string& json) {
    auto frameData = parseMotion(json);
    applyToCharacter(frameData); // Update bone transforms
}

enum EmotionState { CALM, ALERT, ANGRY, PANIC, JOYFUL };

class EmotionController {
    EmotionState current;
    float emotionLevel; // 0.0 – 1.0

public:
    void updateFromContext(float noise, float proximity, float damage);
    EmotionState evaluate();
};

std::map<EmotionState, std::string> emotionToMotion = {
    {CALM, "idle.qtrmotion"},
    {ALERT, "lookaround.qtrmotion"},
    {ANGRY, "rage_strike.qtrmotion"},
    {PANIC, "flee.qtrmotion"},
    {JOYFUL, "dance.qtrmotion"}
};

void updateEmotionAnimations(EmotionState state) {
    currentMotion = emotionToMotion[state];
    playQtrMotion(currentMotion);
}

// Flutter frontend gesture capture
GestureDetector(
  onPanUpdate: (details) {
    sendToEngine("gesture_pan", details.localPosition.dx, details.localPosition.dy);
  },
  onDoubleTap: () {
    sendToEngine("gesture_doubletap", 0, 0);
  },
)

void onGestureReceived(std::string type, float x, float y) {
    if (type == "gesture_pan")
        playQtrMotion("swipe.qtrmotion");
    else if (type == "gesture_doubletap")
        playQtrMotion("teleport.qtrmotion");
}
struct GestureFrame {
    float time;
    std::map<std::string, float> boneAngles;
};

using GestureSequence = std::vector<GestureFrame>;

GestureSequence extractGestureFromReplay(const std::string& replayPath, const std::string& gestureName) {
    GestureSequence sequence;
    std::ifstream in(replayPath);
    std::string line;
    bool capturing = false;

    while (std::getline(in, line)) {
        if (line.find("gesture_start: " + gestureName) != std::string::npos)
            capturing = true;
        else if (line.find("gesture_end: " + gestureName) != std::string::npos)
            break;
        else if (capturing) {
            GestureFrame frame;
            std::istringstream ss(line);
            ss >> frame.time;
            std::string pair;
            while (ss >> pair) {
                auto delim = pair.find(':');
                frame.boneAngles[pair.substr(0, delim)] = std::stof(pair.substr(delim + 1));
            }
            sequence.push_back(frame);
        }
    }
    return sequence;
}

class MimicLibrary {
    std::map<std::string, GestureSequence> learnedGestures;

public:
    void learnGesture(const std::string& name, const GestureSequence& seq) {
        learnedGestures[name] = seq;
    }

    void playGesture(const std::string& name) {
        if (learnedGestures.contains(name)) {
            for (auto& frame : learnedGestures[name]) {
                applyBonePose(frame.boneAngles);
                waitUntil(frame.time); // simple sequencer
            }
        }
    }
};

enum Emotion {
    NEUTRAL, JOYFUL, ANXIOUS, ENRAGED
};

class EmotionLooper {
    Emotion current = NEUTRAL;
    float level = 0.0f;
    float decayRate = 0.05f;

public:
    void trigger(Emotion e, float intensity) {
        if (e == current)
            level = std::min(1.0f, level + intensity);
        else {
            current = e;
            level = intensity;
        }
    }

    void tick(float dt) {
        level -= decayRate * dt;
        if (level <= 0) {
            current = NEUTRAL;
            level = 0;
        }
    }

    Emotion getState() const { return current; }
    float getIntensity() const { return level; }
};

struct ComboMotion {
    std::vector<std::string> motions;
    Emotion requiredEmotion;
};

class ComboMemory {
    std::map<std::string, ComboMotion> combos;

public:
    void defineCombo(const std::string& name, ComboMotion combo) {
        combos[name] = combo;
    }

    void tryTriggerCombo(Emotion current, const std::string& input) {
        if (combos.contains(input)) {
            const auto& combo = combos[input];
            if (combo.requiredEmotion == current) {
                for (auto& motion : combo.motions)
                    playQtrMotion(motion);
            }
        }
    }
};
struct NPCBrain {
    std::string name;
    EmotionLooper emotionState;
    std::map<std::string, GestureSequence> learnedGestures;
    std::vector<std::string> knownCombos;

    void saveToFile(const std::string& path) {
        std::ofstream out(path, std::ios::binary);
        out << name << "\n";
        out << (int)emotionState.getState() << " " << emotionState.getIntensity() << "\n";
        out << learnedGestures.size() << "\n";
        for (auto& [gesture, seq] : learnedGestures) {
            out << gesture << " " << seq.size() << "\n";
            for (auto& frame : seq) {
                out << frame.time;
                for (auto& [bone, angle] : frame.boneAngles)
                    out << " " << bone << ":" << angle;
                out << "\n";
            }
        }
        out << knownCombos.size() << "\n";
        for (auto& combo : knownCombos)
            out << combo << "\n";
    }

    void loadFromFile(const std::string& path) {
        std::ifstream in(path);
        std::getline(in, name);
        int emotion;
        float level;
        in >> emotion >> level;
        emotionState.trigger((Emotion)emotion, level);

        int numGestures;
        in >> numGestures;
        for (int i = 0; i < numGestures; ++i) {
            std::string gesture;
            int frames;
            in >> gesture >> frames;
            GestureSequence seq;
            for (int j = 0; j < frames; ++j) {
                GestureFrame frame;
                in >> frame.time;
                std::string pair;
                while (in.peek() != '\n' && in >> pair) {
                    auto colon = pair.find(':');
                    frame.boneAngles[pair.substr(0, colon)] = std::stof(pair.substr(colon + 1));
                }
                in.ignore();
                seq.push_back(frame);
            }
            learnedGestures[gesture] = seq;
        }

        int numCombos;
        in >> numCombos;
        for (int i = 0; i < numCombos; ++i) {
            std::string combo;
            in >> combo;
            knownCombos.push_back(combo);
        }
    }
};

std::map<std::string, std::string> hotkeyToGesture; // e.g., "F1" → "wave"

void bindHotkeyToGesture(const std::string& hotkey, const std::string& gestureName) {
    hotkeyToGesture[hotkey] = gestureName;
}

void onHotkeyPressed(const std::string& hotkey, NPCBrain& npc) {
    if (hotkeyToGesture.contains(hotkey)) {
        const auto& gesture = hotkeyToGesture[hotkey];
        npc.playGesture(gesture);  // Instant emote trigger
    }
}

struct NPCSyncPacket {
    std::string npcID;
    Emotion currentEmotion;
    std::string gestureNow;
    float emotionLevel;

    std::string serialize() {
        std::ostringstream out;
        out << npcID << " " << (int)currentEmotion << " " << gestureNow << " " << emotionLevel;
        return out.str();
    }

    static NPCSyncPacket deserialize(const std::string& data) {
        NPCSyncPacket pkt;
        std::istringstream in(data);
        int emo;
        in >> pkt.npcID >> emo >> pkt.gestureNow >> pkt.emotionLevel;
        pkt.currentEmotion = (Emotion)emo;
        return pkt;
    }
};

void sendNPCState(NPCBrain& npc, const std::string& id) {
    NPCSyncPacket pkt = {
        .npcID = id,
        .currentEmotion = npc.emotionState.getState(),
        .gestureNow = npc.getCurrentGesture(),
        .emotionLevel = npc.emotionState.getIntensity()
    };
    sendToNetwork(pkt.serialize());
}

void receiveNPCState(const std::string& serialized, std::map<std::string, NPCBrain>& npcs) {
    auto pkt = NPCSyncPacket::deserialize(serialized);
    auto& npc = npcs[pkt.npcID];
    npc.emotionState.trigger(pkt.currentEmotion, pkt.emotionLevel);
    npc.playGesture(pkt.gestureNow);
}
// NPCDNAEditor.hpp
#include <QMainWindow>
#include <QTreeWidget>
#include <QSlider>
#include <QComboBox>
#include "NPCBrain.hpp"

class NPCDNAEditor : public QMainWindow {
    Q_OBJECT
public:
    NPCDNAEditor(QWidget* parent = nullptr);
    void loadFile(const QString& path);

private:
    NPCBrain brain;
    QTreeWidget* gestureTree;
    QComboBox* emotionBox;
    QSlider* emotionLevelSlider;

private slots:
    void onGestureSelected(QTreeWidgetItem*, int);
    void onEmotionChanged(int);
    void onEmotionLevelChanged(int);
    void saveNPCDNA();
};

// NPCDNAEditor.cpp
#include "NPCDNAEditor.hpp"
#include <QFileDialog>
#include <QPushButton>

NPCDNAEditor::NPCDNAEditor(QWidget* parent) : QMainWindow(parent) {
    gestureTree = new QTreeWidget(this);
    emotionBox = new QComboBox(this);
    emotionLevelSlider = new QSlider(Qt::Horizontal, this);
    emotionBox->addItems({"Neutral", "Joyful", "Anxious", "Enraged"});

    QPushButton* saveBtn = new QPushButton("Save", this);
    connect(saveBtn, &QPushButton::clicked, this, &NPCDNAEditor::saveNPCDNA);

    setCentralWidget(gestureTree);
    // Layouts omitted for brevity; attach widgets and signals here
}

void NPCDNAEditor::loadFile(const QString& path) {
    brain.loadFromFile(path.toStdString());
    gestureTree->clear();
    for (auto& [gesture, seq] : brain.learnedGestures)
        gestureTree->addTopLevelItem(new QTreeWidgetItem(QStringList() << QString::fromStdString(gesture)));
}

void NPCDNAEditor::saveNPCDNA() {
    QString path = QFileDialog::getSaveFileName(this, "Save .npcdna");
    brain.saveToFile(path.toStdString());
}

enum Trait {
    AGGRESSIVE,
    CURIOUS,
    TIMID,
    LOYAL,
    CHAOTIC
};

struct PersonalityProfile {
    std::set<Trait> traits;

    bool has(Trait t) const { return traits.contains(t); }

    float emotionBias(Emotion e) const {
        if (e == ENRAGED && has(AGGRESSIVE)) return 1.5f;
        if (e == ANXIOUS && has(TIMID)) return 2.0f;
        return 1.0f;
    }

    std::string toString() const {
        std::string out;
        for (auto t : traits) out += std::to_string(t) + ",";
        return out;
    }

    void fromString(const std::string& data) {
        std::istringstream ss(data);
        std::string tok;
        while (std::getline(ss, tok, ',')) {
            if (!tok.empty()) traits.insert((Trait)std::stoi(tok));
        }
    }
};

class EmotionLooper {
    Emotion current = NEUTRAL;
    float level = 0.0f;
    float decayRate = 0.05f;
    PersonalityProfile profile;

public:
    void setPersonality(const PersonalityProfile& p) { profile = p; }

    void trigger(Emotion e, float intensity) {
        float bias = profile.emotionBias(e);
        if (e == current) level = std::min(1.0f, level + intensity * bias);
        else {
            current = e;
            level = intensity * bias;
        }
    }
    // rest same as before
};

void playQtrScene(const std::string& path) {
    std::ifstream in(path);
    std::string line;
    while (std::getline(in, line)) {
        if (line.starts_with("NPC:")) {
            auto colon = line.find(' ');
            std::string npcName = line.substr(4, colon - 4);
            std::string cmd = line.substr(colon + 1);

            if (cmd.starts_with("play ")) {
                std::string gesture = cmd.substr(5);
                getNPC(npcName).playGesture(gesture);
            } else if (cmd.starts_with("emotion ")) {
                auto parts = split(cmd, ' ');
                Emotion e = parseEmotion(parts[1]);
                float lvl = std::stof(parts[3]);
                getNPC(npcName).emotionState.trigger(e, lvl);
            } else if (cmd.starts_with("say ")) {
                std::string text = cmd.substr(4);
                showDialogue(npcName, text);
            }
        } else if (line.starts_with("wait ")) {
            float duration = parseTime(line.substr(5));
            wait(duration);
        }
    }
}

void exportSceneToVideo(const std::string& scenePath, const std::string& outVideo) {
    auto scene = loadScene(scenePath);
    const int width = 1280, height = 720;
    FILE* ffmpeg = popen(
        "ffmpeg -y -f rawvideo -pixel_format rgb24 -video_size 1280x720 -framerate 30 -i - -c:v libx264 -preset fast -pix_fmt yuv420p output.mp4",
        "w"
    );

    for (int frame = 0; frame < scene.duration * 30; ++frame) {
        renderSceneFrame(scene, frame);
        uint8_t* pixels = captureFrameBuffer(width, height); // get pixel buffer
        fwrite(pixels, 1, width * height * 3, ffmpeg);
        delete[] pixels;
    }
    pclose(ffmpeg);
}

void streamSceneIntoCapsule(const std::string& scenePath) {
    QtrScene scene = loadScene(scenePath);
    for (auto& action : scene.actions) {
        capsuleRuntime.inject("scene_action", serialize(action));
        capsuleRuntime.tick(0.033f);  // advance frame
    }
}

// In GUI Editor Loop
if (ImGui::Begin("Scene Builder")) {
    static char gesture[64] = "";
    static char line[128] = "";

    if (ImGui::InputText("Gesture", gesture, 64) &&
        ImGui::Button("Add Gesture")) {
        currentScene.addAction({"gesture", currentNPC, gesture});
    }

    if (ImGui::InputText("Dialogue", line, 128) &&
        ImGui::Button("Add Line")) {
        currentScene.addAction({"say", currentNPC, line});
    }

    if (ImGui::Button("Save .qtrscene")) {
        currentScene.save("exported_scene.qtrscene");
    }

    if (ImGui::Button("Preview")) {
        playQtrScene("exported_scene.qtrscene");
    }
}
ImGui::End();

std::string queryQuarterGPTLocal(const std::string& prompt) {
    std::string command = "llama-cli --model qtrgpt.gguf --prompt \"" + prompt + "\"";
    std::string result;
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return "";
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe)) result += buffer;
    pclose(pipe);
    return result;
}

void suggestCutsceneViaLLM(const std::string& situation) {
    std::string prompt = "Suggest a .qtrscene cutscene where NPC reacts to this: " + situation;
    std::string script = queryQuarterGPTLocal(prompt);
    std::ofstream out("ai_generated_scene.qtrscene");
    out << script;
    out.close();
    std::cout << "[QuarterGPT] Suggested cutscene saved to ai_generated_scene.qtrscene\n";
}

NPC:Vera play bow
NPC:Vera emotion Joyful level 0.6
wait 1s
NPC:Kael play nod
NPC:Kael say "We’ve waited for you."
wait 2s
