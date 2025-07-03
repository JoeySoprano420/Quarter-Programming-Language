// quarterc.cpp -- QuarterLang (All-in-One)

#include <vector>
#include <iostream>
#include <string>
#include "Lexer.h"
#include "Parser.h"
#include "AST.h"
#include "Binder.h"
#include "Optimizer.h"
#include "CodeGenerator.h"
#include "Bytecode.h"
#include "Runtime.h"
#include "Environment.h"
#include "PackageManager.h"
#include "ErrorHandler.h"
#include "SyntaxHighlighter.h"
#include "Debugger.h"
#include "Indexter.h"
#include "Filer.h"
#include "Formatter.h"
#include "Renderer.h"
#include "LibrarySystem.h"
#include "Injector.h"
#include "Seeder.h"
#include "Encapsulation.h"
#include "Scoper.h"
#include "MemoryHandler.h"
#include "GarbageHandler.h"
#include "TrackerTracer.h"
#include "Conceptualizer.h"
#include "ConfigManager.h"
#include "Manipulator.h"
#include "TransformAgent.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>

// Heuristics for QuarterLang
const std::vector<std::string> quarter_keywords = {
    "quarter", "func", "begin", "end", "let", "const", "if", "else", "loop", "out", "in", "ret", "import"
};

const std::vector<std::string> quarter_operators = {
    ":>", "<:", "->", "<-", "::", "=>"
};

bool containsQuarterKeyword(const std::string& code) {
    for (const auto& keyword : quarter_keywords) {
        std::regex word_regex("\\b" + keyword + "\\b");
        if (std::regex_search(code, word_regex)) {
            return true;
        }
    }
    return false;
}

bool containsQuarterOperator(const std::string& code) {
    for (const auto& op : quarter_operators) {
        if (code.find(op) != std::string::npos) {
            return true;
        }
    }
    return false;
}

// Main detection function
bool isQuarterLang(const std::string& code) {
    int score = 0;

    if (containsQuarterKeyword(code)) score += 2;
    if (containsQuarterOperator(code)) score += 2;

    // Look for "func" function declarations (common in QuarterLang)
    std::regex func_decl(R"(func\s+\w+\s*\()");
    if (std::regex_search(code, func_decl)) score += 2;

    // Heuristic: does code start with "quarter"
    if (code.find("quarter") == 0) score += 1;

    // Heuristic: line contains ":>"
    if (code.find(":>") != std::string::npos) score += 1;

    // At least 4 points => likely QuarterLang
    return score >= 4;
}

// Read file to string
std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Usage: ./lang_detector <filename>
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: ./lang_detector <filename>\n";
        return 1;
    }

    std::string code = readFile(argv[1]);

    if (isQuarterLang(code)) {
        std::cout << "Detected: Quarter Programming Language!\n";
    } else {
        std::cout << "Not QuarterLang (or unsure).\n";
    }

    return 0;
}

#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

// Struct to hold parsed inline function
struct InlineFunc {
    std::string name;
    std::vector<std::string> params;
    std::string body; // single return statement for simplicity
};

// Parse inline functions
std::unordered_map<std::string, InlineFunc> parseInlineFuncs(const std::string& source) {
    std::unordered_map<std::string, InlineFunc> inlines;
    std::regex funcRegex(R"(inline\s+func\s+(\w+)\s*\(([^)]*)\)\s*\{([^}]*)\})");
    auto words_begin = std::sregex_iterator(source.begin(), source.end(), funcRegex);
    auto words_end = std::sregex_iterator();
    for (auto it = words_begin; it != words_end; ++it) {
        std::smatch match = *it;
        InlineFunc f;
        f.name = match[1].str();
        // Split params by ','
        std::istringstream paramStream(match[2].str());
        std::string param;
        while (getline(paramStream, param, ',')) {
            // Remove whitespace
            param.erase(remove_if(param.begin(), param.end(), isspace), param.end());
            if (!param.empty()) f.params.push_back(param);
        }
        // Only support "return ..." as body
        std::regex retRegex(R"(return\s+([^;]+);)");
        std::smatch retMatch;
        if (std::regex_search(match[3].str(), retMatch, retRegex)) {
            f.body = retMatch[1].str();
        }
        inlines[f.name] = f;
    }
    return inlines;
}

// Inline function calls in the main function
std::string inlineCalls(const std::string& source, const std::unordered_map<std::string, InlineFunc>& inlines) {
    std::string result = source;
    for (const auto& [fname, func] : inlines) {
        // Match calls: e.g., add(2, 3)
        std::regex callRegex(fname + R"(\s*\(([^)]*)\))");
        std::smatch match;
        while (std::regex_search(result, match, callRegex)) {
            std::string argsStr = match[1].str();
            // Split args
            std::vector<std::string> args;
            std::istringstream argStream(argsStr);
            std::string arg;
            while (getline(argStream, arg, ',')) {
                arg.erase(remove_if(arg.begin(), arg.end(), isspace), arg.end());
                if (!arg.empty()) args.push_back(arg);
            }
            // Replace parameters in body
            std::string inlined = func.body;
            for (size_t i = 0; i < func.params.size(); ++i) {
                std::regex paramRegex("\\b" + func.params[i] + "\\b");
                inlined = std::regex_replace(inlined, paramRegex, (i < args.size() ? args[i] : ""));
            }
            inlined = "(" + inlined + ")";
            // Replace call in result
            result.replace(match.position(0), match.length(0), inlined);
        }
    }
    return result;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: inliner <file.quarter>\n";
        return 1;
    }
    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Could not open file.\n";
        return 1;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    auto inlines = parseInlineFuncs(source);

    // Remove inline function definitions
    std::regex inlineFuncDef(R"(inline\s+func\s+\w+\s*\([^)]*\)\s*\{[^}]*\})");
    std::string withoutInlines = std::regex_replace(source, inlineFuncDef, "");

    // Inline calls
    std::string inlinedSource = inlineCalls(withoutInlines, inlines);

    std::cout << inlinedSource << std::endl;
    return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

// Basic QuarterLang keywords & types (expand as needed)
const std::vector<std::string> quarter_keywords = {
    "let", "fn", "if", "else", "while", "for", "return", "import",
    "as", "from", "struct", "enum", "match", "break", "continue", "true", "false"
};

const std::vector<std::string> quarter_types = {
    "Int", "Float", "Bool", "Char", "String", "Void"
};

const std::vector<std::string> quarter_operators = {
    "+", "-", "*", "/", "%", "=", "==", "!=", "<", ">", "<=", ">=",
    "&&", "||", "!", ".", "::", "->", ":"
};

// Utility: startsWith for suggestions
bool startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size()
        && std::equal(prefix.begin(), prefix.end(), str.begin());
}

// Collect suggestions given a partial token
std::vector<std::string> getSuggestions(const std::string& partial) {
    std::vector<std::string> suggestions;

    // Keywords
    for (const auto& kw : quarter_keywords) {
        if (startsWith(kw, partial)) suggestions.push_back(kw);
    }
    // Types
    for (const auto& tp : quarter_types) {
        if (startsWith(tp, partial)) suggestions.push_back(tp);
    }
    // Operators (for single char completions, like "=" or "!")
    for (const auto& op : quarter_operators) {
        if (startsWith(op, partial)) suggestions.push_back(op);
    }

    return suggestions;
}

// Simple: get the last word (token) from user input
std::string getLastToken(const std::string& code) {
    if (code.empty()) return "";
    size_t end = code.find_last_not_of(" \t\n");
    if (end == std::string::npos) return "";

    size_t start = code.find_last_of(" \t\n", end);
    if (start == std::string::npos) start = 0;
    else start = start + 1;

    return code.substr(start, end - start + 1);
}

int main() {
    std::cout << "QuarterLang Code Completion Agent\n";
    std::cout << "Type code, partial token at end will be completed. Type 'exit' to quit.\n";

    std::string line;
    while (true) {
        std::cout << "\n> ";
        std::getline(std::cin, line);
        if (line == "exit") break;

        std::string lastToken = getLastToken(line);
        if (lastToken.empty()) {
            std::cout << "(No partial token to complete.)\n";
            continue;
        }

        auto suggestions = getSuggestions(lastToken);
        if (suggestions.empty()) {
            std::cout << "No suggestions.\n";
        } else {
            std::cout << "Suggestions for '" << lastToken << "':\n";
            for (const auto& s : suggestions) {
                std::cout << "  " << s << '\n';
            }
        }
    }
    std::cout << "Goodbye.\n";
    return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

// Basic QuarterLang keywords & types (expand as needed)
const std::vector<std::string> quarter_keywords = {
    "let", "fn", "if", "else", "while", "for", "return", "import",
    "as", "from", "struct", "enum", "match", "break", "continue", "true", "false"
};

const std::vector<std::string> quarter_types = {
    "Int", "Float", "Bool", "Char", "String", "Void"
};

const std::vector<std::string> quarter_operators = {
    "+", "-", "*", "/", "%", "=", "==", "!=", "<", ">", "<=", ">=",
    "&&", "||", "!", ".", "::", "->", ":"
};

// Utility: startsWith for suggestions
bool startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size()
        && std::equal(prefix.begin(), prefix.end(), str.begin());
}

// Collect suggestions given a partial token
std::vector<std::string> getSuggestions(const std::string& partial) {
    std::vector<std::string> suggestions;

    // Keywords
    for (const auto& kw : quarter_keywords) {
        if (startsWith(kw, partial)) suggestions.push_back(kw);
    }
    // Types
    for (const auto& tp : quarter_types) {
        if (startsWith(tp, partial)) suggestions.push_back(tp);
    }
    // Operators (for single char completions, like "=" or "!")
    for (const auto& op : quarter_operators) {
        if (startsWith(op, partial)) suggestions.push_back(op);
    }

    return suggestions;
}

// Simple: get the last word (token) from user input
std::string getLastToken(const std::string& code) {
    if (code.empty()) return "";
    size_t end = code.find_last_not_of(" \t\n");
    if (end == std::string::npos) return "";

    size_t start = code.find_last_of(" \t\n", end);
    if (start == std::string::npos) start = 0;
    else start = start + 1;

    return code.substr(start, end - start + 1);
}

int main() {
    std::cout << "QuarterLang Code Completion Agent\n";
    std::cout << "Type code, partial token at end will be completed. Type 'exit' to quit.\n";

    std::string line;
    while (true) {
        std::cout << "\n> ";
        std::getline(std::cin, line);
        if (line == "exit") break;

        std::string lastToken = getLastToken(line);
        if (lastToken.empty()) {
            std::cout << "(No partial token to complete.)\n";
            continue;
        }

        auto suggestions = getSuggestions(lastToken);
        if (suggestions.empty()) {
            std::cout << "No suggestions.\n";
        } else {
            std::cout << "Suggestions for '" << lastToken << "':\n";
            for (const auto& s : suggestions) {
                std::cout << "  " << s << '\n';
            }
        }
    }
    std::cout << "Goodbye.\n";
    return 0;
}

// QuarterLangComposer.h
#pragma once
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <iostream>

// Forward declaration for AST Node (you would define this elsewhere)
class QuarterASTNode;

// Represents a composed module or program unit
struct ComposedUnit {
    std::string name;
    std::vector<std::shared_ptr<QuarterASTNode>> statements;
    // Add other necessary fields (imports, exports, etc.)

    ComposedUnit(const std::string& n) : name(n) {}
};

class QuarterLangComposer {
public:
    QuarterLangComposer();

    // Compose a full program from parsed AST nodes (modules, functions, etc.)
    void composeProgram(const std::vector<std::shared_ptr<QuarterASTNode>>& nodes);

    // Access composed units (modules/functions/etc.)
    const std::vector<ComposedUnit>& getComposedUnits() const;

    // Debug/utility: print composed structure
    void printComposedUnits() const;

private:
    std::vector<ComposedUnit> composedUnits;
    std::map<std::string, size_t> moduleIndexMap; // For quick lookup

    // Internal: compose a single module or unit
    void composeModule(const std::shared_ptr<QuarterASTNode>& node);

    // Internal: helper to resolve dependencies between units
    void resolveDependencies();

    // ... add more as needed (linking, validation, etc.)
};

// QuarterLangCompactor.cpp
// A simple Compactor for Quarter Programming Language Source Files
// Based on: https://github.com/JoeySoprano420/Quarter-Programming-Language

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>

// Compacts QuarterLang code: removes comments, trims whitespace, compacts code.
class QuarterLangCompactor {
public:
    // Main entry: compact the file from inputPath and write to outputPath
    static bool compactFile(const std::string& inputPath, const std::string& outputPath) {
        std::ifstream inFile(inputPath);
        if (!inFile.is_open()) {
            std::cerr << "Failed to open input file: " << inputPath << std::endl;
            return false;
        }

        std::ofstream outFile(outputPath);
        if (!outFile.is_open()) {
            std::cerr << "Failed to open output file: " << outputPath << std::endl;
            return false;
        }

        std::string line;
        while (std::getline(inFile, line)) {
            std::string compacted = compactLine(line);
            if (!compacted.empty()) {
                outFile << compacted << "\n";
            }
        }

        inFile.close();
        outFile.close();
        return true;
    }

    // Removes comments and unnecessary whitespace from a single line
    static std::string compactLine(const std::string& line) {
        std::string result = line;

        // Remove single-line comments (// ...)
        size_t commentPos = result.find("//");
        if (commentPos != std::string::npos) {
            result = result.substr(0, commentPos);
        }

        // Remove leading/trailing whitespace
        result = std::regex_replace(result, std::regex("^\\s+|\\s+$"), "");

        // Collapse multiple spaces/tabs to single space
        result = std::regex_replace(result, std::regex("\\s+"), " ");

        // Remove redundant spaces around symbols (; , ( ) { } = + - * / < > etc)
        result = std::regex_replace(result, std::regex("\\s*([;:(),{}=+\\-*/<>])\\s*"), "$1");

        // If the line is now empty, skip it.
        return result;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "QuarterLang Compactor\n";
        std::cout << "Usage: " << argv[0] << " <input.quarter> <output.compact.quarter>\n";
        return 1;
    }

    std::string inputPath = argv[1];
    std::string outputPath = argv[2];

    if (QuarterLangCompactor::compactFile(inputPath, outputPath)) {
        std::cout << "Compaction complete. Output written to: " << outputPath << std::endl;
    } else {
        std::cerr << "Compaction failed." << std::endl;
        return 2;
    }

    return 0;
}

#pragma once

#include <string>
#include <memory>
#include "QuarterLangAST.h"      // Assume you have an AST structure
#include "QuarterLangIR.h"       // Assume you have IR representation
#include "QuarterLangRuntime.h"  // Assume you have runtime API

// Base interface for any Adapter
class QuarterLangAdapter {
public:
    virtual ~QuarterLangAdapter() {}

    // Called when the AST is ready
    virtual bool processAST(const QuarterLangAST& ast) = 0;

    // Called when IR is ready (after lowering)
    virtual bool processIR(const QuarterLangIR& ir) = 0;

    // Called to execute/interact with runtime
    virtual bool execute(QuarterLangRuntime& runtime) = 0;

    // Optional: Get adapter name/type
    virtual std::string name() const = 0;
};

// Example: An Adapter for a Binary Emitter
class QuarterBinaryEmitterAdapter : public QuarterLangAdapter {
public:
    bool processAST(const QuarterLangAST& ast) override {
        // (Optional) convert AST to IR or skip
        return true;
    }

    bool processIR(const QuarterLangIR& ir) override {
        // Convert IR to x86_64 binary or similar
        // Emit binary file
        // ...implementation...
        return true;
    }

    bool execute(QuarterLangRuntime& runtime) override {
        // Not needed for emitter
        return true;
    }

    std::string name() const override { return "BinaryEmitter"; }
};

// Example: An Adapter for JIT Execution
class QuarterJITAdapter : public QuarterLangAdapter {
public:
    bool processAST(const QuarterLangAST& ast) override {
        // Lower AST to IR
        // ...implementation...
        return true;
    }

    bool processIR(const QuarterLangIR& ir) override {
        // JIT compile and run
        // ...implementation...
        return true;
    }

    bool execute(QuarterLangRuntime& runtime) override {
        // Kick off runtime, run code
        // ...implementation...
        return true;
    }

    std::string name() const override { return "JITAdapter"; }
};

// QuarterLangBinder.hpp

#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <stack>
#include <iostream>

// ---- Symbol Representation ----
enum class SymbolKind { Variable, Function, Unknown };

struct Symbol {
    std::string name;
    SymbolKind kind;
    int scopeLevel; // For debugging, diagnostics
    // Add type, value, etc. as needed

    Symbol(std::string n, SymbolKind k, int level)
        : name(std::move(n)), kind(k), scopeLevel(level) {}
};

// ---- Scope Representation ----
class Scope {
public:
    std::unordered_map<std::string, Symbol> symbols;
    std::shared_ptr<Scope> parent;
    int level;

    Scope(std::shared_ptr<Scope> parentScope = nullptr, int lvl = 0)
        : parent(parentScope), level(lvl) {}

    bool addSymbol(const Symbol& sym) {
        if (symbols.count(sym.name)) return false; // Already defined here
        symbols[sym.name] = sym;
        return true;
    }

    Symbol* resolve(const std::string& name) {
        if (symbols.count(name)) return &symbols[name];
        if (parent) return parent->resolve(name);
        return nullptr;
    }
};

// ---- Binder Core ----
class Binder {
    std::shared_ptr<Scope> currentScope;
    int scopeDepth;

public:
    Binder() : currentScope(std::make_shared<Scope>()), scopeDepth(0) {}

    void enterScope() {
        currentScope = std::make_shared<Scope>(currentScope, ++scopeDepth);
    }

    void exitScope() {
        if (currentScope->parent) {
            currentScope = currentScope->parent;
            --scopeDepth;
        }
    }

    bool declare(const std::string& name, SymbolKind kind) {
        Symbol sym(name, kind, scopeDepth);
        bool ok = currentScope->addSymbol(sym);
        if (!ok) {
            std::cerr << "[Binder] Error: '" << name << "' already defined in this scope (Level " << scopeDepth << ")\n";
        }
        return ok;
    }

    Symbol* lookup(const std::string& name) {
        return currentScope->resolve(name);
    }

    // ---- For AST integration ----
    // Walk over your AST, calling declare/lookup as you process variable/function definitions and references.
    // Example stubs:
    void bindVariable(const std::string& name) {
        declare(name, SymbolKind::Variable);
    }

    void bindFunction(const std::string& name) {
        declare(name, SymbolKind::Function);
    }

    void useIdentifier(const std::string& name) {
        Symbol* sym = lookup(name);
        if (!sym) {
            std::cerr << "[Binder] Error: Use of undefined identifier '" << name << "'\n";
        }
        // else: all good, can annotate AST node with symbol info if desired
    }
};

// ====== QUARTERLANG READER: Core File Reader/Loader ======
// Handles: File reading, string streaming, error management
// Usage: QuarterReader qr("mycode.quarter"); std::string src = qr.getSource();

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

class QuarterReader {
public:
    // Constructor: Takes filename, attempts to read immediately
    explicit QuarterReader(const std::string& filename)
        : source_(""), loaded_(false)
    {
        loadFile(filename);
    }

    // Fetches the source code as a string
    const std::string& getSource() const {
        return source_;
    }

    // Checks if file loaded successfully
    bool isLoaded() const {
        return loaded_;
    }

private:
    std::string source_;
    bool loaded_;

    // Loads the file content into the source string
    void loadFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::in | std::ios::binary);
        if (!file) {
            loaded_ = false;
            throw std::runtime_error("QuarterReader: Unable to open file: " + filename);
        }

        std::ostringstream ss;
        ss << file.rdbuf();   // Slurp entire file
        source_ = ss.str();
        loaded_ = true;
    }
};

// ====== DEMO USAGE ======
// int main() {
//     try {
//         QuarterReader reader("example.quarter");
//         if (reader.isLoaded()) {
//             std::cout << "Quarter Source Loaded:\n";
//             std::cout << reader.getSource() << std::endl;
//         }
//     } catch (const std::exception& ex) {
//         std::cerr << "Error: " << ex.what() << std::endl;
//         return 1;
//     }
//     return 0;
// }

// quarter_protocol.hpp
#pragma once
#include <string>
#include <vector>
#include <unordered_map>

// === Protocol for all QuarterLang Components ===
class IQuarterComponent {
public:
    virtual ~IQuarterComponent() = default;

    // --- Initialization & Configuration ---
    virtual void initialize(const std::unordered_map<std::string, std::string>& config) = 0;

    // --- Core IO ---
    virtual void loadSource(const std::string& src) = 0;
    virtual std::string getComponentName() const = 0;

    // --- Core Processing ---
    virtual void process() = 0; // Main routine (tokenize, parse, exec, etc.)
    virtual bool hasError() const = 0;
    virtual std::string getError() const = 0;

    // --- Input/Output Protocols ---
    virtual void setInput(const std::string& input) = 0;
    virtual std::string getOutput() const = 0;

    // --- Diagnostics ---
    virtual std::string getDiagnostics() const = 0;
    virtual void reset() = 0;
};

#pragma once
#include <string>
#include <unordered_map>
#include <iostream>
#include <variant>

// --- Typedefs for QuarterLang variable types ---
using QuarterInt = int;
using QuarterText = std::string;
using QuarterDG = double; // 'dg' is probably double/decimal/float
using QuarterValue = std::variant<QuarterInt, QuarterText, QuarterDG>;

// --- The QuarterLang Environment ---
class QuarterEnvironment {
public:
    // Set variable (overwrite or create)
    void set(const std::string& name, const QuarterValue& value) {
        variables[name] = value;
    }

    // Get variable (throws if not found)
    QuarterValue get(const std::string& name) const {
        auto it = variables.find(name);
        if (it == variables.end())
            throw std::runtime_error("QuarterLang: variable '" + name + "' not found.");
        return it->second;
    }

    // Check if variable exists
    bool has(const std::string& name) const {
        return variables.count(name) > 0;
    }

    // Output (say)
    void say(const QuarterValue& value) const {
        std::visit([](auto&& val) {
            std::cout << val << std::endl;
        }, value);
    }

    // (OPTIONAL) Dump all variables for debug
    void dump() const {
        std::cout << "=== QuarterLang ENV ===" << std::endl;
        for (const auto& [name, value] : variables) {
            std::cout << name << " = ";
            std::visit([](auto&& val) { std::cout << val; }, value);
            std::cout << std::endl;
        }
    }

private:
    std::unordered_map<std::string, QuarterValue> variables;
};

// QuarterLang Runtime Engine — Core Implementation
// Author: Violet Aura Creations (2025)

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <variant>
#include <functional>
#include <stdexcept>
#include <sstream>
#include <stack>

// === Value Representation ===
enum class QType { NUMBER, STRING, BOOL, NONE };

struct QValue {
    QType type = QType::NONE;
    double numValue = 0;
    std::string strValue;
    bool boolValue = false;

    QValue() : type(QType::NONE) {}
    QValue(double v) : type(QType::NUMBER), numValue(v) {}
    QValue(const std::string& v) : type(QType::STRING), strValue(v) {}
    QValue(bool v) : type(QType::BOOL), boolValue(v) {}

    std::string toString() const {
        switch (type) {
            case QType::NUMBER: return std::to_string(numValue);
            case QType::STRING: return strValue;
            case QType::BOOL:   return boolValue ? "true" : "false";
            default: return "none";
        }
    }
};

// === Variable Environment ===
using QEnv = std::unordered_map<std::string, QValue>;

// === AST Node Types (Simplified) ===
enum class QNodeType { PROGRAM, VAL, VAR, ASSIGN, SAY, ASK, IF, LOOP, FUNC_DEF, FUNC_CALL, BLOCK };

// === AST Node Definition ===
struct QNode {
    QNodeType type;
    std::string name;
    std::vector<std::shared_ptr<QNode>> children;
    QValue value;
    std::vector<std::string> params; // for func_def
    // Extra fields can be added as needed

    QNode(QNodeType t) : type(t) {}
};

// === Runtime Context / Call Stack Frame ===
struct QFrame {
    QEnv vars;
    std::unordered_map<std::string, std::shared_ptr<QNode>> functions;
    QFrame* parent = nullptr;

    QValue getVar(const std::string& name) {
        if (vars.count(name)) return vars[name];
        if (parent) return parent->getVar(name);
        throw std::runtime_error("Variable '" + name + "' not defined.");
    }
    void setVar(const std::string& name, const QValue& val) {
        if (vars.count(name)) vars[name] = val;
        else if (parent) parent->setVar(name, val);
        else throw std::runtime_error("Variable '" + name + "' not defined.");
    }
    void defineVar(const std::string& name, const QValue& val) {
        vars[name] = val;
    }
    void defineFunc(const std::string& name, std::shared_ptr<QNode> def) {
        functions[name] = def;
    }
    std::shared_ptr<QNode> getFunc(const std::string& name) {
        if (functions.count(name)) return functions[name];
        if (parent) return parent->getFunc(name);
        throw std::runtime_error("Function '" + name + "' not defined.");
    }
};

// === Interpreter / Runtime Engine ===
class QuarterRuntime {
public:
    QuarterRuntime(std::shared_ptr<QNode> program)
        : programNode(program), globalFrame(new QFrame{}) {}

    void run() {
        execNode(programNode.get(), globalFrame.get());
    }

private:
    std::shared_ptr<QNode> programNode;
    std::unique_ptr<QFrame> globalFrame;

    QValue execNode(QNode* node, QFrame* frame) {
        switch (node->type) {
            case QNodeType::PROGRAM:
                for (auto& child : node->children) execNode(child.get(), frame);
                break;

            case QNodeType::VAL:
            case QNodeType::VAR: {
                QValue val = node->value;
                frame->defineVar(node->name, val);
                break;
            }
            case QNodeType::ASSIGN: {
                QValue val = evalExpr(node->children[0].get(), frame);
                frame->setVar(node->name, val);
                break;
            }
            case QNodeType::SAY: {
                QValue val = evalExpr(node->children[0].get(), frame);
                std::cout << val.toString() << std::endl;
                break;
            }
            case QNodeType::ASK: {
                std::string input;
                std::getline(std::cin, input);
                frame->defineVar(node->name, QValue(input));
                break;
            }
            case QNodeType::IF: {
                QValue cond = evalExpr(node->children[0].get(), frame);
                if (cond.boolValue) execNode(node->children[1].get(), frame);
                else if (node->children.size() > 2)
                    execNode(node->children[2].get(), frame); // else branch
                break;
            }
            case QNodeType::LOOP: {
                while (evalExpr(node->children[0].get(), frame).boolValue) {
                    execNode(node->children[1].get(), frame);
                }
                break;
            }
            case QNodeType::FUNC_DEF: {
                frame->defineFunc(node->name, std::make_shared<QNode>(*node));
                break;
            }
            case QNodeType::FUNC_CALL: {
                auto funcNode = frame->getFunc(node->name);
                QFrame localFrame;
                localFrame.parent = frame;
                // assign params
                for (size_t i = 0; i < funcNode->params.size(); ++i) {
                    QValue argVal = evalExpr(node->children[i].get(), frame);
                    localFrame.defineVar(funcNode->params[i], argVal);
                }
                return execNode(funcNode->children[0].get(), &localFrame);
            }
            case QNodeType::BLOCK: {
                for (auto& child : node->children) execNode(child.get(), frame);
                break;
            }
            default:
                throw std::runtime_error("Unknown node type in runtime.");
        }
        return QValue();
    }

    // === Expression Evaluator ===
    QValue evalExpr(QNode* node, QFrame* frame) {
        // For demo: numbers, variables, string literals, bools
        if (node->type == QNodeType::VAL || node->type == QNodeType::VAR) {
            return frame->getVar(node->name);
        }
        // Extend: arithmetic, comparisons, etc.
        return node->value;
    }
};

// === Demo: Build and Run a Simple Quarter Program ===
std::shared_ptr<QNode> demoQuarterProgram() {
    // star
    //   val x = 10
    //   var y = "hello"
    //   say y
    // end

    auto program = std::make_shared<QNode>(QNodeType::PROGRAM);
    auto valX = std::make_shared<QNode>(QNodeType::VAL);
    valX->name = "x";
    valX->value = QValue(10);

    auto varY = std::make_shared<QNode>(QNodeType::VAR);
    varY->name = "y";
    varY->value = QValue("hello");

    auto sayY = std::make_shared<QNode>(QNodeType::SAY);
    auto yExpr = std::make_shared<QNode>(QNodeType::VAR);
    yExpr->name = "y";
    sayY->children.push_back(yExpr);

    program->children = {valX, varY, sayY};
    return program;
}

int main() {
    try {
        auto prog = demoQuarterProgram();
        QuarterRuntime runtime(prog);
        runtime.run();
    } catch (const std::exception& ex) {
        std::cerr << "[QuarterLang Runtime Error] " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}

// quarter_runner.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>

// Forward declaration for QuarterLang Interpreter (assume available in your project)
class QuarterInterpreter {
public:
    // Returns exit code
    int run(const std::string& source, const std::vector<std::string>& args);
};

// Utility: Read entire file into string
std::string readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::in);
    if (!file) throw std::runtime_error("Error: Could not open file '" + filename + "'");
    std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return contents;
}

// Print usage/help
void printUsage(const std::string& exeName) {
    std::cout << "QuarterLang Runner\n";
    std::cout << "Usage: " << exeName << " <script.quarter> [args...]\n";
    std::cout << "  Runs a QuarterLang script.\n";
}

// MAIN
int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    try {
        std::string filename = argv[1];
        std::vector<std::string> scriptArgs;
        for (int i = 2; i < argc; ++i)
            scriptArgs.emplace_back(argv[i]);

        std::string source = readFile(filename);

        // Run QuarterLang interpreter/VM
        QuarterInterpreter interpreter;
        int exitCode = interpreter.run(source, scriptArgs);
        return exitCode;

    } catch (const std::exception& ex) {
        std::cerr << "[Runner Error] " << ex.what() << std::endl;
        return 2;
    }
}

// quarter_interpreter.cpp
#include <iostream>
#include <string>
#include <vector>

class QuarterInterpreter {
public:
    int run(const std::string& source, const std::vector<std::string>& args) {
        std::cout << "[QuarterLang RUNNER]\n";
        std::cout << "Received Source (" << source.size() << " chars)\n";
        std::cout << "Arguments: ";
        for (const auto& arg : args) std::cout << arg << " ";
        std::cout << "\n\n";
        std::cout << "=== Begin Script ===\n";
        std::cout << source << "\n";
        std::cout << "===  End Script  ===\n";
        // Placeholder: integrate real execution logic here
        return 0;
    }
};

// quarterpm.cpp
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <nlohmann/json.hpp>

// If using Linux/Mac, add: #include <unistd.h>
namespace fs = std::filesystem;
using json = nlohmann::json;

const std::string QUARTER_DIR = "quarter_packages";
const std::string QUARTER_CONFIG = "quarter.json";
const std::string REGISTRY_FILE = "quarter_registry.json"; // Local registry for demo

void ensureDirectory(const std::string& dir) {
    if (!fs::exists(dir)) fs::create_directory(dir);
}

json loadConfig(const std::string& file) {
    std::ifstream f(file);
    if (!f.is_open()) return json{};
    json j;
    f >> j;
    return j;
}

void saveConfig(const std::string& file, const json& j) {
    std::ofstream f(file);
    f << j.dump(4);
}

bool installPackage(const std::string& pkgName, json& config, const json& registry) {
    if (!registry.contains(pkgName)) {
        std::cout << "Package not found in registry.\n";
        return false;
    }
    std::string url = registry[pkgName]["url"];
    std::string version = registry[pkgName]["version"];
    std::string destDir = QUARTER_DIR + "/" + pkgName;
    ensureDirectory(QUARTER_DIR);

    std::cout << "Cloning " << url << " into " << destDir << "\n";
    std::string cmd = "git clone " + url + " " + destDir;
    int res = system(cmd.c_str());
    if (res != 0) {
        std::cout << "Clone failed.\n";
        return false;
    }
    config["dependencies"][pkgName] = version;
    saveConfig(QUARTER_CONFIG, config);
    std::cout << "Installed " << pkgName << "@" << version << "\n";
    return true;
}

void uninstallPackage(const std::string& pkgName, json& config) {
    std::string dir = QUARTER_DIR + "/" + pkgName;
    if (fs::exists(dir)) {
        fs::remove_all(dir);
        config["dependencies"].erase(pkgName);
        saveConfig(QUARTER_CONFIG, config);
        std::cout << "Removed " << pkgName << "\n";
    } else {
        std::cout << "Not installed: " << pkgName << "\n";
    }
}

void listPackages(const json& config) {
    std::cout << "Installed Packages:\n";
    if (!config.contains("dependencies")) {
        std::cout << "No packages installed.\n";
        return;
    }
    for (auto& [k, v] : config["dependencies"].items()) {
        std::cout << " - " << k << "@" << v << "\n";
    }
}

void searchRegistry(const std::string& term, const json& registry) {
    std::cout << "Registry search for: " << term << "\n";
    for (auto& [k, v] : registry.items()) {
        if (k.find(term) != std::string::npos) {
            std::cout << " - " << k << " (" << v["version"] << ") - " << v["desc"] << "\n";
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "QuarterPM Usage:\n";
        std::cout << "  quarterpm install <package>\n";
        std::cout << "  quarterpm uninstall <package>\n";
        std::cout << "  quarterpm list\n";
        std::cout << "  quarterpm search <term>\n";
        return 1;
    }
    std::string cmd = argv[1];
    json config = loadConfig(QUARTER_CONFIG);
    json registry = loadConfig(REGISTRY_FILE);

    if (cmd == "install" && argc == 3) {
        std::string pkg = argv[2];
        installPackage(pkg, config, registry);
    } else if (cmd == "uninstall" && argc == 3) {
        std::string pkg = argv[2];
        uninstallPackage(pkg, config);
    } else if (cmd == "list") {
        listPackages(config);
    } else if (cmd == "search" && argc == 3) {
        searchRegistry(argv[2], registry);
    } else {
        std::cout << "Unknown or malformed command.\n";
    }
    return 0;
}

// QuarterLangWrapper.hpp
#pragma once
#include <string>
#include <vector>
#include <memory>

class QuarterLangVM;

class QuarterLangWrapper {
public:
    // Create a new VM instance
    QuarterLangWrapper();

    // Destroy the VM
    ~QuarterLangWrapper();

    // Load and execute a .quarter source file
    bool executeFile(const std::string& filename);

    // Execute Quarter code from a string
    bool executeString(const std::string& code);

    // Set arguments (for scripts with parameters)
    void setArgs(const std::vector<std::string>& args);

    // Get the last output (if supported by your VM)
    std::string getLastOutput() const;

    // Reset the VM
    void reset();

private:
    std::unique_ptr<QuarterLangVM> vm;
};

// QuarterIO.hpp
#pragma once
#include <iostream>
#include <string>
#include <unordered_map>

class QuarterIO {
public:
    // Reference to current variable table (could be improved with custom Value type)
    std::unordered_map<std::string, std::string>& variables;

    explicit QuarterIO(std::unordered_map<std::string, std::string>& vars) : variables(vars) {}

    // say "some text"
    void say(const std::string& text) {
        std::cout << text << std::endl;
    }

    // ask varName "Prompt text"
    void ask(const std::string& varName, const std::string& prompt) {
        std::cout << prompt;
        std::string input;
        std::getline(std::cin, input);
        variables[varName] = input;
    }
};

// When visiting a SAY node:
qio.say(node.text);

// When visiting an ASK node:
qio.ask(node.varName, node.promptText);

#include <iostream>
#include <filesystem>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <vector>
#include <string>
#include <fstream>

namespace fs = std::filesystem;

// --- Configurable Pipeline Commands ---
const std::string BUILD_CMD = "make";      // or "cmake --build .", or custom build
const std::string TEST_CMD = "./run_tests"; // Replace with your actual test runner
const std::string DEPLOY_CMD = "./deploy.sh"; // Could be scp, rsync, or your deploy script

// --- Directories to Watch ---
const std::vector<std::string> WATCH_DIRS = { "src", "include", "tests" };

// --- Polling interval in seconds ---
const int POLL_INTERVAL = 3;

// --- Helper: Get Latest Modification Time ---
std::time_t get_latest_write_time(const std::vector<std::string>& dirs) {
    std::time_t latest = 0;
    for (const auto& dir : dirs) {
        if (!fs::exists(dir)) continue;
        for (auto& p : fs::recursive_directory_iterator(dir)) {
            if (!fs::is_regular_file(p)) continue;
            auto ftime = fs::last_write_time(p);
            auto cftime = decltype(ftime)::clock::to_time_t(ftime);
            if (cftime > latest) latest = cftime;
        }
    }
    return latest;
}

// --- Run System Command, Return Success ---
bool run_command(const std::string& cmd) {
    std::cout << "[RUN] " << cmd << std::endl;
    int result = std::system(cmd.c_str());
    return (result == 0);
}

int main() {
    std::cout << "QuarterLang CI/CD Pipeline Runner\n";
    std::time_t last_checked = get_latest_write_time(WATCH_DIRS);

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(POLL_INTERVAL));
        std::time_t latest = get_latest_write_time(WATCH_DIRS);
        if (latest > last_checked) {
            std::cout << "\n=== Change detected! Running pipeline... ===\n";
            last_checked = latest;

            // --- 1. Build Step ---
            if (!run_command(BUILD_CMD)) {
                std::cerr << "[FAIL] Build failed. Skipping tests/deploy.\n";
                continue;
            }
            std::cout << "[PASS] Build complete.\n";

            // --- 2. Test Step ---
            if (!run_command(TEST_CMD)) {
                std::cerr << "[FAIL] Tests failed. Skipping deploy.\n";
                continue;
            }
            std::cout << "[PASS] All tests passed.\n";

            // --- 3. Deploy Step ---
            if (!run_command(DEPLOY_CMD)) {
                std::cerr << "[FAIL] Deploy failed.\n";
                continue;
            }
            std::cout << "[PASS] Deploy complete!\n";
            std::cout << "=============================================\n";
        } else {
            std::cout << ".";
            std::cout.flush();
        }
    }

    return 0;
}

// main.cpp — QuarterLang Caller (Driver)
// Reads a .quarter file, processes, and runs it.

#include <iostream>
#include <fstream>
#include <string>

// Include your QuarterLang headers
#include "Lexer.h"
#include "Parser.h"
#include "AST.h"
#include "CodeGenerator.h"
#include "VM.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: quarter <source_file.quarter>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Cannot open file '" << filename << "'" << std::endl;
        return 1;
    }

    // Read source code into a string
    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // === PHASE 1: Lexing ===
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    // === PHASE 2: Parsing ===
    Parser parser(tokens);
    std::shared_ptr<ASTNode> program = parser.parse();

    if (!program) {
        std::cerr << "Parse Error. Exiting." << std::endl;
        return 1;
    }

    // === PHASE 3: Code Generation ===
    CodeGenerator codegen;
    auto ir = codegen.generate(program);

    // === PHASE 4: Execution (VM) ===
    VM vm;
    vm.run(ir);

    // Success
    return 0;
}

// ========================
// QuarterLang Optimizer.hpp
// ========================

#pragma once
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <variant>
#include <optional>

// --- AST Node Definitions (You should replace/extend these with your own) ---

enum class NodeKind { Program, Block, VarDecl, Assign, BinaryOp, Literal, Identifier, If, While, Call, Return };

struct ASTNode;
using AST = std::shared_ptr<ASTNode>;

struct ASTNode {
    NodeKind kind;
    std::vector<AST> children;
    std::string name;        // Used for variables, function names, etc.
    std::string op;          // For binary operators ("+", "-", etc.)
    std::variant<int, double, std::string> value; // For literals
    // ...other fields as needed (types, etc.)
    ASTNode(NodeKind k) : kind(k) {}
};

using SymbolTable = std::unordered_map<std::string, std::variant<int, double, std::string>>;

// --- Optimizer Class ---

class QuarterOptimizer {
public:
    QuarterOptimizer() = default;

    // Entrypoint: optimize whole AST in-place
    AST optimize(const AST& root) {
        SymbolTable globals;
        return optimizeNode(root, globals);
    }

private:
    // Recursively optimize nodes
    AST optimizeNode(const AST& node, SymbolTable& symbols) {
        switch (node->kind) {
            case NodeKind::Program:
            case NodeKind::Block: {
                auto newNode = std::make_shared<ASTNode>(node->kind);
                for (const auto& child : node->children) {
                    auto optChild = optimizeNode(child, symbols);
                    if (optChild) newNode->children.push_back(optChild);
                }
                return newNode;
            }
            case NodeKind::VarDecl:
            case NodeKind::Assign: {
                auto newNode = std::make_shared<ASTNode>(node->kind);
                newNode->name = node->name;
                auto rhs = optimizeNode(node->children[0], symbols);
                newNode->children.push_back(rhs);
                // Track assignment for constant propagation
                if (rhs->kind == NodeKind::Literal) {
                    symbols[newNode->name] = rhs->value;
                }
                return newNode;
            }
            case NodeKind::BinaryOp: {
                // Try constant folding
                auto left = optimizeNode(node->children[0], symbols);
                auto right = optimizeNode(node->children[1], symbols);
                if (left->kind == NodeKind::Literal && right->kind == NodeKind::Literal) {
                    return foldConstants(node->op, left->value, right->value);
                }
                // x + 0 or 0 + x optimization
                if (node->op == "+" || node->op == "-") {
                    if (isLiteralZero(left)) return right;
                    if (isLiteralZero(right) && node->op == "+") return left;
                }
                // x * 1 or 1 * x, x * 0, 0 * x
                if (node->op == "*") {
                    if (isLiteralOne(left)) return right;
                    if (isLiteralOne(right)) return left;
                    if (isLiteralZero(left) || isLiteralZero(right)) return makeLiteral(0);
                }
                auto newNode = std::make_shared<ASTNode>(NodeKind::BinaryOp);
                newNode->op = node->op;
                newNode->children = {left, right};
                return newNode;
            }
            case NodeKind::Literal: {
                return node;
            }
            case NodeKind::Identifier: {
                // Constant propagation
                auto it = symbols.find(node->name);
                if (it != symbols.end()) {
                    return makeLiteral(it->second);
                }
                return node;
            }
            case NodeKind::If: {
                auto cond = optimizeNode(node->children[0], symbols);
                // Dead code elimination for constant condition
                if (cond->kind == NodeKind::Literal) {
                    if (isTrue(cond->value)) {
                        return optimizeNode(node->children[1], symbols); // Then branch
                    } else if (node->children.size() > 2) {
                        return optimizeNode(node->children[2], symbols); // Else branch
                    } else {
                        return nullptr;
                    }
                }
                auto newNode = std::make_shared<ASTNode>(NodeKind::If);
                newNode->children.push_back(cond);
                for (size_t i = 1; i < node->children.size(); ++i) {
                    auto optBranch = optimizeNode(node->children[i], symbols);
                    if (optBranch) newNode->children.push_back(optBranch);
                }
                return newNode;
            }
            case NodeKind::While: {
                auto cond = optimizeNode(node->children[0], symbols);
                if (cond->kind == NodeKind::Literal && !isTrue(cond->value)) {
                    // while (false): Remove entire loop
                    return nullptr;
                }
                auto newNode = std::make_shared<ASTNode>(NodeKind::While);
                newNode->children.push_back(cond);
                auto body = optimizeNode(node->children[1], symbols);
                if (body) newNode->children.push_back(body);
                return newNode;
            }
            case NodeKind::Call: {
                auto newNode = std::make_shared<ASTNode>(NodeKind::Call);
                newNode->name = node->name;
                for (const auto& arg : node->children)
                    newNode->children.push_back(optimizeNode(arg, symbols));
                return newNode;
            }
            case NodeKind::Return: {
                auto newNode = std::make_shared<ASTNode>(NodeKind::Return);
                newNode->children.push_back(optimizeNode(node->children[0], symbols));
                return newNode;
            }
            default:
                return node;
        }
    }

    // --- Optimization helpers ---
    AST foldConstants(const std::string& op, const std::variant<int, double, std::string>& l, const std::variant<int, double, std::string>& r) {
        // Only int/double folding (string folding can be implemented similarly)
        if (l.index() == 0 && r.index() == 0) { // int
            int lv = std::get<int>(l), rv = std::get<int>(r);
            if (op == "+") return makeLiteral(lv + rv);
            if (op == "-") return makeLiteral(lv - rv);
            if (op == "*") return makeLiteral(lv * rv);
            if (op == "/") return makeLiteral(rv != 0 ? lv / rv : 0); // div by 0 is UB
        }
        if (l.index() == 1 && r.index() == 1) { // double
            double lv = std::get<double>(l), rv = std::get<double>(r);
            if (op == "+") return makeLiteral(lv + rv);
            if (op == "-") return makeLiteral(lv - rv);
            if (op == "*") return makeLiteral(lv * rv);
            if (op == "/") return makeLiteral(rv != 0 ? lv / rv : 0.0);
        }
        // ...add more folding as needed
        return nullptr;
    }
    static bool isLiteralZero(const AST& n) {
        if (n->kind != NodeKind::Literal) return false;
        if (n->value.index() == 0) return std::get<int>(n->value) == 0;
        if (n->value.index() == 1) return std::get<double>(n->value) == 0.0;
        return false;
    }
    static bool isLiteralOne(const AST& n) {
        if (n->kind != NodeKind::Literal) return false;
        if (n->value.index() == 0) return std::get<int>(n->value) == 1;
        if (n->value.index() == 1) return std::get<double>(n->value) == 1.0;
        return false;
    }
    static bool isTrue(const std::variant<int, double, std::string>& v) {
        if (v.index() == 0) return std::get<int>(v) != 0;
        if (v.index() == 1) return std::get<double>(v) != 0.0;
        if (v.index() == 2) return !std::get<std::string>(v).empty();
        return false;
    }
    static AST makeLiteral(const std::variant<int, double, std::string>& v) {
        auto n = std::make_shared<ASTNode>(NodeKind::Literal);
        n->value = v;
        return n;
    }
    static AST makeLiteral(int v) {
        auto n = std::make_shared<ASTNode>(NodeKind::Literal);
        n->value = v;
        return n;
    }
};

// --- Usage ---
// auto optimizedRoot = QuarterOptimizer().optimize(parsedAST);

// TLCM.hpp
#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <memory>

// === IR & AST Node Types ===
enum class NodeKind { FUNC, VAR, CALL, CONST, ASSIGN, RETURN, BLOCK };

// Forward-declare IR Node
struct IRNode;

// === Symbol Information ===
struct Symbol {
    std::string name;
    NodeKind kind;
    int address; // Offset in code or memory
    std::string type;
    Symbol(const std::string& n, NodeKind k, int addr, const std::string& t)
        : name(n), kind(k), address(addr), type(t) {}
};

// === Reference Entry ===
struct Reference {
    std::string symbolName;
    int patchLocation; // Where to patch in code
    NodeKind useKind;
};

// === Conversion Mapping Entry ===
struct ConversionMapEntry {
    std::shared_ptr<IRNode> node;
    int binaryOffset; // Where the output code sits
};

// === IR Node Example (minimal, for demonstration) ===
struct IRNode {
    NodeKind kind;
    std::string value;
    std::vector<std::shared_ptr<IRNode>> children;
    IRNode(NodeKind k, const std::string& v) : kind(k), value(v) {}
};

// === TLCM Core ===
class TLCM {
public:
    // Symbol table: name → Symbol
    std::unordered_map<std::string, Symbol> symbols;

    // List of unresolved references to patch after all symbols discovered
    std::vector<Reference> unresolvedRefs;

    // IR Node → Output offset
    std::vector<ConversionMapEntry> conversionMap;

    // === Register a symbol (function/variable)
    void registerSymbol(const std::string& name, NodeKind kind, int addr, const std::string& type) {
        symbols[name] = Symbol(name, kind, addr, type);
    }

    // === Register unresolved reference (to be patched later)
    void registerReference(const std::string& symbolName, int patchLoc, NodeKind useKind) {
        unresolvedRefs.push_back({ symbolName, patchLoc, useKind });
    }

    // === Map IR node to output offset
    void mapConversion(const std::shared_ptr<IRNode>& node, int outputOffset) {
        conversionMap.push_back({ node, outputOffset });
    }

    // === Link and Patch all unresolved references
    void linkAndPatch() {
        for (auto& ref : unresolvedRefs) {
            if (symbols.count(ref.symbolName)) {
                int address = symbols[ref.symbolName].address;
                // Patch code here; in a real linker, write address into binary/code buffer.
                std::cout << "[PATCH] Ref '" << ref.symbolName
                          << "' at offset " << ref.patchLocation
                          << " -> address " << address << std::endl;
            } else {
                std::cerr << "[ERROR] Unresolved symbol: " << ref.symbolName << std::endl;
            }
        }
        unresolvedRefs.clear();
    }

    // === Example: Print symbol table
    void printSymbols() {
        std::cout << "=== Symbol Table ===" << std::endl;
        for (const auto& [name, sym] : symbols) {
            std::cout << sym.name << " (" << (sym.kind == NodeKind::FUNC ? "FUNC" : "VAR")
                      << ") @ " << sym.address << ", type: " << sym.type << std::endl;
        }
    }
};

// Example usage:

// Build IR
auto mainFunc = std::make_shared<IRNode>(NodeKind::FUNC, "main");
auto callFoo = std::make_shared<IRNode>(NodeKind::CALL, "foo");

// Create TLCM
TLCM tlcm;

// Register symbols
tlcm.registerSymbol("main", NodeKind::FUNC, 0, "void");
tlcm.registerSymbol("foo", NodeKind::FUNC, 10, "void");

// Add unresolved reference (e.g., a call to foo at code offset 5)
tlcm.registerReference("foo", 5, NodeKind::CALL);

// Map IR nodes to output offsets
tlcm.mapConversion(mainFunc, 0);
tlcm.mapConversion(callFoo, 5);

// After all emission, patch references:
tlcm.linkAndPatch();

// Print symbol table for diagnostics
tlcm.printSymbols();

// ========================
// QuarterLang Optimizer.hpp
// ========================

#pragma once
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <variant>
#include <optional>

// --- AST Node Definitions (You should replace/extend these with your own) ---

enum class NodeKind { Program, Block, VarDecl, Assign, BinaryOp, Literal, Identifier, If, While, Call, Return };

struct ASTNode;
using AST = std::shared_ptr<ASTNode>;

struct ASTNode {
    NodeKind kind;
    std::vector<AST> children;
    std::string name;        // Used for variables, function names, etc.
    std::string op;          // For binary operators ("+", "-", etc.)
    std::variant<int, double, std::string> value; // For literals
    // ...other fields as needed (types, etc.)
    ASTNode(NodeKind k) : kind(k) {}
};

using SymbolTable = std::unordered_map<std::string, std::variant<int, double, std::string>>;

// --- Optimizer Class ---

class QuarterOptimizer {
public:
    QuarterOptimizer() = default;

    // Entrypoint: optimize whole AST in-place
    AST optimize(const AST& root) {
        SymbolTable globals;
        return optimizeNode(root, globals);
    }

private:
    // Recursively optimize nodes
    AST optimizeNode(const AST& node, SymbolTable& symbols) {
        switch (node->kind) {
            case NodeKind::Program:
            case NodeKind::Block: {
                auto newNode = std::make_shared<ASTNode>(node->kind);
                for (const auto& child : node->children) {
                    auto optChild = optimizeNode(child, symbols);
                    if (optChild) newNode->children.push_back(optChild);
                }
                return newNode;
            }
            case NodeKind::VarDecl:
            case NodeKind::Assign: {
                auto newNode = std::make_shared<ASTNode>(node->kind);
                newNode->name = node->name;
                auto rhs = optimizeNode(node->children[0], symbols);
                newNode->children.push_back(rhs);
                // Track assignment for constant propagation
                if (rhs->kind == NodeKind::Literal) {
                    symbols[newNode->name] = rhs->value;
                }
                return newNode;
            }
            case NodeKind::BinaryOp: {
                // Try constant folding
                auto left = optimizeNode(node->children[0], symbols);
                auto right = optimizeNode(node->children[1], symbols);
                if (left->kind == NodeKind::Literal && right->kind == NodeKind::Literal) {
                    return foldConstants(node->op, left->value, right->value);
                }
                // x + 0 or 0 + x optimization
                if (node->op == "+" || node->op == "-") {
                    if (isLiteralZero(left)) return right;
                    if (isLiteralZero(right) && node->op == "+") return left;
                }
                // x * 1 or 1 * x, x * 0, 0 * x
                if (node->op == "*") {
                    if (isLiteralOne(left)) return right;
                    if (isLiteralOne(right)) return left;
                    if (isLiteralZero(left) || isLiteralZero(right)) return makeLiteral(0);
                }
                auto newNode = std::make_shared<ASTNode>(NodeKind::BinaryOp);
                newNode->op = node->op;
                newNode->children = {left, right};
                return newNode;
            }
            case NodeKind::Literal: {
                return node;
            }
            case NodeKind::Identifier: {
                // Constant propagation
                auto it = symbols.find(node->name);
                if (it != symbols.end()) {
                    return makeLiteral(it->second);
                }
                return node;
            }
            case NodeKind::If: {
                auto cond = optimizeNode(node->children[0], symbols);
                // Dead code elimination for constant condition
                if (cond->kind == NodeKind::Literal) {
                    if (isTrue(cond->value)) {
                        return optimizeNode(node->children[1], symbols); // Then branch
                    } else if (node->children.size() > 2) {
                        return optimizeNode(node->children[2], symbols); // Else branch
                    } else {
                        return nullptr;
                    }
                }
                auto newNode = std::make_shared<ASTNode>(NodeKind::If);
                newNode->children.push_back(cond);
                for (size_t i = 1; i < node->children.size(); ++i) {
                    auto optBranch = optimizeNode(node->children[i], symbols);
                    if (optBranch) newNode->children.push_back(optBranch);
                }
                return newNode;
            }
            case NodeKind::While: {
                auto cond = optimizeNode(node->children[0], symbols);
                if (cond->kind == NodeKind::Literal && !isTrue(cond->value)) {
                    // while (false): Remove entire loop
                    return nullptr;
                }
                auto newNode = std::make_shared<ASTNode>(NodeKind::While);
                newNode->children.push_back(cond);
                auto body = optimizeNode(node->children[1], symbols);
                if (body) newNode->children.push_back(body);
                return newNode;
            }
            case NodeKind::Call: {
                auto newNode = std::make_shared<ASTNode>(NodeKind::Call);
                newNode->name = node->name;
                for (const auto& arg : node->children)
                    newNode->children.push_back(optimizeNode(arg, symbols));
                return newNode;
            }
            case NodeKind::Return: {
                auto newNode = std::make_shared<ASTNode>(NodeKind::Return);
                newNode->children.push_back(optimizeNode(node->children[0], symbols));
                return newNode;
            }
            default:
                return node;
        }
    }

    // --- Optimization helpers ---
    AST foldConstants(const std::string& op, const std::variant<int, double, std::string>& l, const std::variant<int, double, std::string>& r) {
        // Only int/double folding (string folding can be implemented similarly)
        if (l.index() == 0 && r.index() == 0) { // int
            int lv = std::get<int>(l), rv = std::get<int>(r);
            if (op == "+") return makeLiteral(lv + rv);
            if (op == "-") return makeLiteral(lv - rv);
            if (op == "*") return makeLiteral(lv * rv);
            if (op == "/") return makeLiteral(rv != 0 ? lv / rv : 0); // div by 0 is UB
        }
        if (l.index() == 1 && r.index() == 1) { // double
            double lv = std::get<double>(l), rv = std::get<double>(r);
            if (op == "+") return makeLiteral(lv + rv);
            if (op == "-") return makeLiteral(lv - rv);
            if (op == "*") return makeLiteral(lv * rv);
            if (op == "/") return makeLiteral(rv != 0 ? lv / rv : 0.0);
        }
        // ...add more folding as needed
        return nullptr;
    }
    static bool isLiteralZero(const AST& n) {
        if (n->kind != NodeKind::Literal) return false;
        if (n->value.index() == 0) return std::get<int>(n->value) == 0;
        if (n->value.index() == 1) return std::get<double>(n->value) == 0.0;
        return false;
    }
    static bool isLiteralOne(const AST& n) {
        if (n->kind != NodeKind::Literal) return false;
        if (n->value.index() == 0) return std::get<int>(n->value) == 1;
        if (n->value.index() == 1) return std::get<double>(n->value) == 1.0;
        return false;
    }
    static bool isTrue(const std::variant<int, double, std::string>& v) {
        if (v.index() == 0) return std::get<int>(v) != 0;
        if (v.index() == 1) return std::get<double>(v) != 0.0;
        if (v.index() == 2) return !std::get<std::string>(v).empty();
        return false;
    }
    static AST makeLiteral(const std::variant<int, double, std::string>& v) {
        auto n = std::make_shared<ASTNode>(NodeKind::Literal);
        n->value = v;
        return n;
    }
    static AST makeLiteral(int v) {
        auto n = std::make_shared<ASTNode>(NodeKind::Literal);
        n->value = v;
        return n;
    }
};

// --- Usage ---
// auto optimizedRoot = QuarterOptimizer().optimize(parsedAST);

enum class QOp {
    Q_MOV,      // mov reg, value
    Q_ADD,      // add reg, value
    Q_SAY,      // print reg or string
    Q_EXIT,     // exit program
    // ...expand as needed
};
struct QInstr {
    QOp op;
    int reg;
    int value;
    std::string str;
};

#include <fstream>
#include <vector>
#include <cstdint>
#include <string>
#include <cstring>

// Minimal IR definition (expand as needed)
enum class QOp { Q_SAY, Q_EXIT };
struct QInstr {
    QOp op;
    std::string str; // Only needed for Q_SAY
};

// Util: Write raw bytes to buffer/file
void write_bytes(std::vector<uint8_t>& buf, const void* src, size_t n) {
    const uint8_t* b = static_cast<const uint8_t*>(src);
    buf.insert(buf.end(), b, b + n);
}

// Emit minimal 64-bit Windows PE executable with a hardcoded string
void emit_win64_pe(const std::vector<QInstr>& program, const std::string& outfile) {
    std::vector<uint8_t> bin;

    // --- Minimal PE Header + .text section ---
    // This is a "naked" PE with just enough for Hello World.
    // For clarity, we use a fixed shell and patch the message in.

    // (PE shell is public domain: https://github.com/corkami/pocs/blob/master/PE/minshell.asm)
    const uint8_t pe_template[] = {
        // DOS Header (e_lfanew = 0x80)
        0x4D,0x5A,0x90,0x00,0x03,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,
        0xB8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        // ...skipped...
        // At offset 0x80: PE signature
        // -- Insert the rest of the PE template (see corkami/pocs for full bytes)
    };
    bin.insert(bin.end(), std::begin(pe_template), std::end(pe_template));

    // --- Insert Message ---
    std::string message = "Hello, Quarter!\n";
    for (const auto& instr : program) {
        if (instr.op == QOp::Q_SAY && !instr.str.empty())
            message = instr.str;
    }

    // Patch the message at the right offset (e.g., at 0x200 in .data section)
    size_t msg_offset = 0x200; // adjust to match template
    if (bin.size() < msg_offset + message.size() + 1)
        bin.resize(msg_offset + message.size() + 1, 0);
    std::memcpy(bin.data() + msg_offset, message.c_str(), message.size() + 1);

    // --- Patch string pointer in code (e.g., mov rcx, offset string) ---
    // This depends on your shellcode; for a real project, you’ll need to
    // generate code for each IR instruction and fixup references accordingly.

    // --- Write to file ---
    std::ofstream out(outfile, std::ios::binary);
    out.write((const char*)bin.data(), bin.size());
    out.close();
}

int main() {
    // Example IR: say "Hello from Quarter Binary Emitter!", then exit
    std::vector<QInstr> prog = {
        { QOp::Q_SAY, "Hello from Quarter Binary Emitter!" },
        { QOp::Q_EXIT, "" }
    };
    emit_win64_pe(prog, "quarter_output.exe");
    return 0;
}

// AST Node Definitions (minimal for demo)
#include <string>
#include <vector>
#include <memory>

enum class NodeKind { PROGRAM, VAR_DECL, ASSIGN, NUMBER, IDENT, BIN_OP, SAY, BLOCK };

struct ASTNode {
    NodeKind kind;
    virtual ~ASTNode() = default;
};

using ASTPtr = std::shared_ptr<ASTNode>;

struct NumberNode : ASTNode {
    int value;
    NumberNode(int v) : value(v) { kind = NodeKind::NUMBER; }
};

struct IdentNode : ASTNode {
    std::string name;
    IdentNode(const std::string& n) : name(n) { kind = NodeKind::IDENT; }
};

struct BinOpNode : ASTNode {
    std::string op;
    ASTPtr left, right;
    BinOpNode(const std::string& o, ASTPtr l, ASTPtr r)
        : op(o), left(l), right(r) { kind = NodeKind::BIN_OP; }
};

struct VarDeclNode : ASTNode {
    std::string name;
    ASTPtr value;
    VarDeclNode(const std::string& n, ASTPtr v) : name(n), value(v) { kind = NodeKind::VAR_DECL; }
};

struct AssignNode : ASTNode {
    std::string name;
    ASTPtr value;
    AssignNode(const std::string& n, ASTPtr v) : name(n), value(v) { kind = NodeKind::ASSIGN; }
};

struct SayNode : ASTNode {
    ASTPtr expr;
    SayNode(ASTPtr e) : expr(e) { kind = NodeKind::SAY; }
};

struct BlockNode : ASTNode {
    std::vector<ASTPtr> stmts;
    BlockNode(const std::vector<ASTPtr>& s) : stmts(s) { kind = NodeKind::BLOCK; }
};

#include <iostream>
#include <sstream>

class QuarterCodeGen {
public:
    QuarterCodeGen() {}

    std::string generate(const ASTPtr& node) {
        std::ostringstream out;
        emit(node, out, 0);
        return out.str();
    }

private:
    void emit(const ASTPtr& node, std::ostringstream& out, int indent) {
        if (!node) return;
        switch (node->kind) {
        case NodeKind::BLOCK:
            for (const auto& stmt : static_cast<BlockNode*>(node.get())->stmts) {
                emit(stmt, out, indent);
            }
            break;
        case NodeKind::VAR_DECL: {
            auto* n = static_cast<VarDeclNode*>(node.get());
            out << std::string(indent, ' ') << "int " << n->name << " = ";
            emit(n->value, out, 0);
            out << ";\n";
            break;
        }
        case NodeKind::ASSIGN: {
            auto* n = static_cast<AssignNode*>(node.get());
            out << std::string(indent, ' ') << n->name << " = ";
            emit(n->value, out, 0);
            out << ";\n";
            break;
        }
        case NodeKind::NUMBER: {
            auto* n = static_cast<NumberNode*>(node.get());
            out << n->value;
            break;
        }
        case NodeKind::IDENT: {
            auto* n = static_cast<IdentNode*>(node.get());
            out << n->name;
            break;
        }
        case NodeKind::BIN_OP: {
            auto* n = static_cast<BinOpNode*>(node.get());
            out << "(";
            emit(n->left, out, 0);
            out << " " << n->op << " ";
            emit(n->right, out, 0);
            out << ")";
            break;
        }
        case NodeKind::SAY: {
            auto* n = static_cast<SayNode*>(node.get());
            out << std::string(indent, ' ') << "std::cout << ";
            emit(n->expr, out, 0);
            out << " << std::endl;\n";
            break;
        }
        default:
            // TODO: more node kinds
            break;
        }
    }
};

int main() {
    // star
    //   val x = 5
    //   x = x + 2
    //   say x
    // end

    ASTPtr program = std::make_shared<BlockNode>(std::vector<ASTPtr>{
        std::make_shared<VarDeclNode>("x", std::make_shared<NumberNode>(5)),
        std::make_shared<AssignNode>("x", std::make_shared<BinOpNode>("+",
            std::make_shared<IdentNode>("x"),
            std::make_shared<NumberNode>(2)
        )),
        std::make_shared<SayNode>(std::make_shared<IdentNode>("x"))
    });

    QuarterCodeGen codegen;
    std::string output = codegen.generate(program);

    std::cout << "// QuarterLang Generated C++\n";
    std::cout << "#include <iostream>\n\nint main() {\n";
    std::cout << output;
    std::cout << "}\n";
    return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <variant>

// === Define OpCodes === //
enum class OpCode : uint8_t {
    NOP = 0x00,
    LOAD_CONST = 0x01,
    LOAD_VAR   = 0x02,
    SET_VAR    = 0x03,
    SAY        = 0x04,
    ADD        = 0x05,
    SUB        = 0x06,
    MUL        = 0x07,
    DIV        = 0x08,
    JUMP       = 0x09,
    JUMP_IF_FALSE = 0x0A,
    END        = 0xFF
};

// === Bytecode Value Type (int, double, string) === //
using QValue = std::variant<int, double, std::string>;

// === IR Instruction Structure === //
struct Instruction {
    OpCode op;
    std::vector<int> int_args;      // For integer operands (e.g., value index, var index, jump address)
    std::vector<QValue> data_args;  // For literal constants (if needed)

    Instruction(OpCode op_) : op(op_) {}
    Instruction(OpCode op_, int arg) : op(op_), int_args{arg} {}
    Instruction(OpCode op_, QValue val) : op(op_), data_args{val} {}
    Instruction(OpCode op_, int arg, QValue val) : op(op_), int_args{arg}, data_args{val} {}
};

// === Quarter Bytecode Program === //
struct BytecodeProgram {
    std::vector<QValue> constants;          // Pool of constants
    std::vector<std::string> variables;     // Variable names table (for mapping indices)
    std::vector<Instruction> instructions;  // The IR/bytecode stream

    void emit(const Instruction& instr) {
        instructions.push_back(instr);
    }
    void dump() const {
        for (size_t i = 0; i < instructions.size(); ++i) {
            std::cout << i << ": ";
            const auto& instr = instructions[i];
            std::cout << static_cast<int>(instr.op);
            for (int arg : instr.int_args) std::cout << " " << arg;
            for (const auto& val : instr.data_args) {
                std::visit([](auto&& v) { std::cout << " [" << v << "]"; }, val);
            }
            std::cout << "\n";
        }
    }
};

// QuarterLang AST Core — C++
// Immersive, expandable, and ready for parsing

#pragma once
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <variant>

// ---------- Base Node Type ----------
struct ASTNode {
    virtual ~ASTNode() = default;
};

// ---------- Expressions ----------
struct Expr : public ASTNode {};
using ExprPtr = std::shared_ptr<Expr>;

// --- Literal: Number, Text, Boolean ---
struct NumberExpr : public Expr {
    double value;
    NumberExpr(double v) : value(v) {}
};

struct TextExpr : public Expr {
    std::string value;
    TextExpr(const std::string& v) : value(v) {}
};

struct BoolExpr : public Expr {
    bool value;
    BoolExpr(bool v) : value(v) {}
};

struct IdentifierExpr : public Expr {
    std::string name;
    IdentifierExpr(const std::string& n) : name(n) {}
};

// --- Binary Operations ---
enum class BinaryOp {
    Add, Sub, Mul, Div, Mod,
    Eq, Neq, Lt, Gt, Leq, Geq,
    And, Or
};

struct BinaryExpr : public Expr {
    BinaryOp op;
    ExprPtr left;
    ExprPtr right;
    BinaryExpr(BinaryOp o, ExprPtr l, ExprPtr r) : op(o), left(std::move(l)), right(std::move(r)) {}
};

// --- Unary Operations ---
enum class UnaryOp {
    Negate, Not
};

struct UnaryExpr : public Expr {
    UnaryOp op;
    ExprPtr expr;
    UnaryExpr(UnaryOp o, ExprPtr e) : op(o), expr(std::move(e)) {}
};

// --- Function Call ---
struct CallExpr : public Expr {
    std::string callee;
    std::vector<ExprPtr> args;
    CallExpr(const std::string& n, std::vector<ExprPtr> a)
        : callee(n), args(std::move(a)) {}
};

// ---------- Statements ----------
struct Stmt : public ASTNode {};
using StmtPtr = std::shared_ptr<Stmt>;

// --- Variable Declaration ---
struct VarDeclStmt : public Stmt {
    bool isConst; // true for 'val', false for 'var'
    std::string name;
    std::optional<ExprPtr> initExpr;
    VarDeclStmt(bool c, const std::string& n, std::optional<ExprPtr> i)
        : isConst(c), name(n), initExpr(std::move(i)) {}
};

// --- Assignment ---
struct AssignStmt : public Stmt {
    std::string name;
    ExprPtr value;
    AssignStmt(const std::string& n, ExprPtr v) : name(n), value(std::move(v)) {}
};

// --- Say Statement (Output) ---
struct SayStmt : public Stmt {
    ExprPtr expr;
    SayStmt(ExprPtr e) : expr(std::move(e)) {}
};

// --- Ask Statement (Input) ---
struct AskStmt : public Stmt {
    std::string prompt;
    std::string varName;
    AskStmt(const std::string& p, const std::string& v) : prompt(p), varName(v) {}
};

// --- If Statement ---
struct IfStmt : public Stmt {
    ExprPtr condition;
    std::vector<StmtPtr> thenBody;
    std::vector<StmtPtr> elseBody;
    IfStmt(ExprPtr cond, std::vector<StmtPtr> t, std::vector<StmtPtr> e)
        : condition(std::move(cond)), thenBody(std::move(t)), elseBody(std::move(e)) {}
};

// --- Match Statement (Switch-like) ---
struct MatchCase {
    ExprPtr pattern;
    std::vector<StmtPtr> body;
};

struct MatchStmt : public Stmt {
    ExprPtr expr;
    std::vector<MatchCase> cases;
    std::vector<StmtPtr> defaultBody;
    MatchStmt(ExprPtr e, std::vector<MatchCase> c, std::vector<StmtPtr> d)
        : expr(std::move(e)), cases(std::move(c)), defaultBody(std::move(d)) {}
};

// --- Loop Statement ---
struct LoopStmt : public Stmt {
    std::string varName;
    ExprPtr start;
    ExprPtr end;
    std::vector<StmtPtr> body;
    LoopStmt(const std::string& v, ExprPtr s, ExprPtr e, std::vector<StmtPtr> b)
        : varName(v), start(std::move(s)), end(std::move(e)), body(std::move(b)) {}
};

// --- Block (star ... end) ---
struct BlockStmt : public Stmt {
    std::vector<StmtPtr> body;
    BlockStmt(std::vector<StmtPtr> stmts) : body(std::move(stmts)) {}
};

// --- Program Root (AST) ---
struct Program : public ASTNode {
    std::vector<StmtPtr> statements;
    Program(std::vector<StmtPtr> s) : statements(std::move(s)) {}
};

#pragma once
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include "lexer.h"  // Assumes you have a lexer/token system

// --- Token Type Alias for Convenience ---
using TokenList = std::vector<Token>;

// === AST Node Types ===
enum class NodeKind {
    BLOCK,        // star ... end
    VAL,          // val name = expr
    VAR,          // var name = expr
    SAY,          // say expr
    NUMBER,       // literal
    STRING,       // literal
    IDENTIFIER,   // variable usage
    ASSIGN,       // name = expr
    IF,           // if ... then ... else ... end
    LOOP,         // loop ... end
    MATCH,        // match ... case ... end
    CALL,         // function call
    UNKNOWN
};

// === AST Base ===
struct ASTNode {
    NodeKind kind;
    int line;
    virtual ~ASTNode() = default;
};
using AST = std::shared_ptr<ASTNode>;

// === Specific AST Nodes ===
struct BlockNode : ASTNode {
    std::vector<AST> statements;
    BlockNode(int l) { kind = NodeKind::BLOCK; line = l; }
};
struct ValNode : ASTNode {
    std::string name;
    AST value;
    ValNode(const std::string& n, AST v, int l) : name(n), value(v) {
        kind = NodeKind::VAL; line = l;
    }
};
struct VarNode : ASTNode {
    std::string name;
    AST value;
    VarNode(const std::string& n, AST v, int l) : name(n), value(v) {
        kind = NodeKind::VAR; line = l;
    }
};
struct SayNode : ASTNode {
    AST value;
    SayNode(AST v, int l) : value(v) { kind = NodeKind::SAY; line = l; }
};
struct NumberNode : ASTNode {
    std::string value;
    NumberNode(const std::string& v, int l) : value(v) { kind = NodeKind::NUMBER; line = l; }
};
struct StringNode : ASTNode {
    std::string value;
    StringNode(const std::string& v, int l) : value(v) { kind = NodeKind::STRING; line = l; }
};
struct IdentifierNode : ASTNode {
    std::string name;
    IdentifierNode(const std::string& n, int l) : name(n) { kind = NodeKind::IDENTIFIER; line = l; }
};
struct AssignNode : ASTNode {
    std::string name;
    AST value;
    AssignNode(const std::string& n, AST v, int l) : name(n), value(v) { kind = NodeKind::ASSIGN; line = l; }
};

// You can expand for IF, LOOP, MATCH, CALL, etc.


// === Parser ===
class Parser {
    const TokenList& tokens;
    size_t pos;

    Token peek(int ahead = 0) const {
        if (pos + ahead < tokens.size()) return tokens[pos + ahead];
        return tokens.back(); // EOF token
    }
    Token consume() { return tokens[pos++]; }
    bool match(const std::string& val) {
        if (peek().value == val) { consume(); return true; }
        return false;
    }
    void expect(const std::string& val) {
        if (peek().value != val) throw std::runtime_error(
            "Expected '" + val + "' at line " + std::to_string(peek().line));
        consume();
    }

public:
    Parser(const TokenList& toks) : tokens(toks), pos(0) {}

    AST parse() {
        // Expect file to start with 'star' and end with 'end'
        expect("star");
        auto block = std::make_shared<BlockNode>(peek().line);
        while (peek().value != "end" && peek().type != TokenType::END_OF_FILE) {
            block->statements.push_back(parse_statement());
        }
        expect("end");
        return block;
    }

    AST parse_statement() {
        Token t = peek();
        if (t.value == "val") return parse_val();
        if (t.value == "var") return parse_var();
        if (t.value == "say") return parse_say();
        if (t.type == TokenType::IDENTIFIER && peek(1).value == "=") return parse_assign();
        throw std::runtime_error("Unknown statement at line " + std::to_string(t.line));
    }

    AST parse_val() {
        Token t = consume(); // val
        Token name = consume(); // identifier
        expect("=");
        AST val = parse_expression();
        return std::make_shared<ValNode>(name.value, val, t.line);
    }
    AST parse_var() {
        Token t = consume(); // var
        Token name = consume(); // identifier
        expect("=");
        AST val = parse_expression();
        return std::make_shared<VarNode>(name.value, val, t.line);
    }
    AST parse_say() {
        Token t = consume(); // say
        AST val = parse_expression();
        return std::make_shared<SayNode>(val, t.line);
    }
    AST parse_assign() {
        Token name = consume(); // identifier
        consume(); // =
        AST val = parse_expression();
        return std::make_shared<AssignNode>(name.value, val, name.line);
    }

    AST parse_expression() {
        Token t = peek();
        if (t.type == TokenType::NUMBER) {
            consume(); return std::make_shared<NumberNode>(t.value, t.line);
        }
        if (t.type == TokenType::STRING) {
            consume(); return std::make_shared<StringNode>(t.value, t.line);
        }
        if (t.type == TokenType::IDENTIFIER) {
            consume(); return std::make_shared<IdentifierNode>(t.value, t.line);
        }
        throw std::runtime_error("Unexpected expression at line " + std::to_string(t.line));
    }
};

#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <unordered_set>

enum class TokenType {
    KEYWORD, IDENTIFIER, NUMBER, STRING, SYMBOL,
    COMMENT, END_OF_FILE
};

struct Token {
    TokenType type;
    std::string value;
    int line;
};

class Lexer {
public:
    Lexer(const std::string& src) : src(src), pos(0), line(1) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        while (true) {
            Token t = nextToken();
            tokens.push_back(t);
            if (t.type == TokenType::END_OF_FILE) break;
        }
        return tokens;
    }

private:
    const std::string src;
    size_t pos;
    int line;

    const std::unordered_set<std::string> keywords = {
        "star","end","val","var","say","loop","if","else","while","match","case","break","continue","return","func"
    };

    char peek() const { return pos < src.size() ? src[pos] : '\0'; }
    char get() { return pos < src.size() ? src[pos++] : '\0'; }
    void skipWhitespace() {
        while (isspace(peek())) {
            if (peek() == '\n') ++line;
            get();
        }
    }
    bool isIdentifierStart(char c) { return std::isalpha(c) || c == '_'; }
    bool isIdentifierChar(char c) { return std::isalnum(c) || c == '_'; }

    Token nextToken() {
        skipWhitespace();

        char c = peek();
        if (c == '\0') return {TokenType::END_OF_FILE, "", line};

        // Comments
        if (c == '/') {
            if (src[pos + 1] == '/') {
                std::string comment;
                while (peek() != '\n' && peek() != '\0') comment += get();
                return {TokenType::COMMENT, comment, line};
            } else if (src[pos + 1] == '*') {
                pos += 2; // Skip /*
                std::string comment = "/*";
                while (pos < src.size() && !(peek() == '*' && src[pos + 1] == '/')) {
                    if (peek() == '\n') ++line;
                    comment += get();
                }
                if (pos < src.size()) {
                    comment += "*";
                    comment += "/";
                    pos += 2;
                }
                return {TokenType::COMMENT, comment, line};
            }
        }

        // Strings
        if (c == '"') {
            std::string str;
            get(); // skip "
            while (peek() != '"' && peek() != '\0') {
                if (peek() == '\\') {
                    get(); // skip \
                    char esc = get();
                    if (esc == 'n') str += '\n';
                    else if (esc == 't') str += '\t';
                    else str += esc;
                } else {
                    str += get();
                }
            }
            get(); // skip ending "
            return {TokenType::STRING, str, line};
        }

        // Numbers
        if (isdigit(c)) {
            std::string num;
            bool has_dot = false;
            while (isdigit(peek()) || peek() == '.') {
                if (peek() == '.') {
                    if (has_dot) break;
                    has_dot = true;
                }
                num += get();
            }
            return {TokenType::NUMBER, num, line};
        }

        // Identifiers/Keywords
        if (isIdentifierStart(c)) {
            std::string id;
            while (isIdentifierChar(peek())) id += get();
            if (keywords.count(id)) return {TokenType::KEYWORD, id, line};
            else return {TokenType::IDENTIFIER, id, line};
        }

        // Symbols (multi-char: ==, !=, <=, >=, ->, =>)
        std::string symbol;
        symbol += get();
        char next = peek();
        if ((symbol == "=" || symbol == "!" || symbol == "<" || symbol == ">" || symbol == "-") && 
            (next == '=' || (symbol == "-" && next == '>'))) {
            symbol += get();
        } else if (symbol == "=" && next == '>') {
            symbol += get();
        }
        return {TokenType::SYMBOL, symbol, line};
    }
};

// === Example usage ===
int main() {
    std::string code = R"(star
    val x = 42
    say "Hello, Quarter!"
    // this is a comment
    end)";
    Lexer lexer(code);
    auto tokens = lexer.tokenize();
    for (auto& t : tokens) {
        std::cout << "Line " << t.line << ": ";
        switch (t.type) {
            case TokenType::KEYWORD: std::cout << "[KEYWORD] "; break;
            case TokenType::IDENTIFIER: std::cout << "[IDENTIFIER] "; break;
            case TokenType::NUMBER: std::cout << "[NUMBER] "; break;
            case TokenType::STRING: std::cout << "[STRING] "; break;
            case TokenType::SYMBOL: std::cout << "[SYMBOL] "; break;
            case TokenType::COMMENT: std::cout << "[COMMENT] "; break;
            case TokenType::END_OF_FILE: std::cout << "[EOF] "; break;
        }
        std::cout << "'" << t.value << "'\n";
    }
    return 0;
}

// === QuarterLang Indexter ===
// Indexter: Symbol Table and Semantic Lookup System

#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <optional>

// ---- Symbol Metadata Structure ----
struct QSymbolInfo {
    std::string name;           // The identifier
    std::string type;           // "int", "text", "dg", "func", etc.
    int scopeLevel;             // Nesting or block depth
    int declLine;               // Declaration line number
    // Additional fields: location, modifiers, etc.
};

// ---- Indexter Class ----
class QIndexter {
public:
    QIndexter() : currentScope(0) {}

    // Enter a new block scope (e.g., after 'star')
    void enterScope() { currentScope++; }
    // Exit a block scope (e.g., at 'end')
    void exitScope() {
        // Remove all symbols declared at this scope
        for (auto it = index.begin(); it != index.end(); ) {
            if (it->second.scopeLevel == currentScope)
                it = index.erase(it);
            else
                ++it;
        }
        if (currentScope > 0) currentScope--;
    }

    // Declare a symbol
    bool declare(const std::string& name, const std::string& type, int line) {
        if (index.count(name) && index[name].scopeLevel == currentScope) {
            // Already declared in this scope
            return false;
        }
        index[name] = QSymbolInfo{name, type, currentScope, line};
        scopeStack[currentScope].push_back(name);
        return true;
    }

    // Lookup symbol info (searches outward through scopes)
    std::optional<QSymbolInfo> lookup(const std::string& name) const {
        auto it = index.find(name);
        if (it != index.end())
            return it->second;
        return std::nullopt;
    }

    // Get current scope depth
    int getScopeLevel() const { return currentScope; }

    // List all symbols in current scope
    std::vector<QSymbolInfo> symbolsInScope() const {
        std::vector<QSymbolInfo> result;
        auto it = scopeStack.find(currentScope);
        if (it != scopeStack.end()) {
            for (const auto& name : it->second) {
                auto found = index.find(name);
                if (found != index.end())
                    result.push_back(found->second);
            }
        }
        return result;
    }

private:
    int currentScope;
    std::unordered_map<std::string, QSymbolInfo> index;
    std::unordered_map<int, std::vector<std::string>> scopeStack;
};

// =======================
// QuarterLang ErrorHandler
// =======================

#ifndef QUARTER_ERROR_HANDLER_H
#define QUARTER_ERROR_HANDLER_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

// Error severity levels
enum class ErrorLevel {
    INFO,
    WARNING,
    ERROR,
    FATAL
};

// Struct to represent an error
struct QuarterError {
    ErrorLevel level;
    std::string message;
    std::string filename;
    int line;
    int column;
    std::string codeSnippet;

    QuarterError(ErrorLevel lvl,
                 const std::string& msg,
                 const std::string& file = "",
                 int ln = -1, int col = -1,
                 const std::string& snippet = "")
        : level(lvl), message(msg), filename(file), line(ln), column(col), codeSnippet(snippet) {}
};

class ErrorHandler {
private:
    std::vector<QuarterError> errors;
    bool verbose = true;
public:
    void report(ErrorLevel level, const std::string& message,
                const std::string& filename = "",
                int line = -1, int column = -1,
                const std::string& codeSnippet = "") {
        QuarterError err(level, message, filename, line, column, codeSnippet);
        errors.push_back(err);
        if (verbose || level == ErrorLevel::FATAL) {
            printError(err);
        }
        if (level == ErrorLevel::FATAL) {
            std::cerr << "QuarterLang: Fatal error. Compilation aborted." << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    void printError(const QuarterError& err) const {
        std::ostringstream oss;
        oss << "[QuarterLang ";
        switch (err.level) {
            case ErrorLevel::INFO:    oss << "INFO"; break;
            case ErrorLevel::WARNING: oss << "WARNING"; break;
            case ErrorLevel::ERROR:   oss << "ERROR"; break;
            case ErrorLevel::FATAL:   oss << "FATAL"; break;
        }
        oss << "]";
        if (!err.filename.empty()) oss << " in " << err.filename;
        if (err.line >= 0) oss << " @ line " << err.line;
        if (err.column >= 0) oss << ":" << err.column;
        oss << " - " << err.message << "\n";
        if (!err.codeSnippet.empty()) {
            oss << "  >> " << err.codeSnippet << "\n";
            if (err.column > 0) {
                oss << "     " << std::setw(err.column + 3) << "^" << "\n";
            }
        }
        std::cerr << oss.str();
    }

    void showAllErrors() const {
        if (errors.empty()) {
            std::cout << "No errors reported.\n";
            return;
        }
        for (const auto& err : errors) printError(err);
    }

    bool hasErrors() const {
        for (const auto& e : errors)
            if (e.level == ErrorLevel::ERROR || e.level == ErrorLevel::FATAL) return true;
        return false;
    }
    void setVerbose(bool v) { verbose = v; }
    void clear() { errors.clear(); }
    int errorCount() const { return (int)errors.size(); }
};

#endif // QUARTER_ERROR_HANDLER_H

// QuarterLang Syntax Highlighter — C++
// Console/Terminal version using ANSI colors

#include <iostream>
#include <string>
#include <unordered_set>
#include <cctype>
#include <sstream>

namespace QuarterLang {

// --- ANSI Color Codes for terminal output ---
const std::string RESET  = "\033[0m";
const std::string KEYWORD = "\033[1;35m";   // Magenta/Bold
const std::string IDENT   = "\033[1;36m";   // Cyan/Bold
const std::string NUMBER  = "\033[1;33m";   // Yellow/Bold
const std::string STRING  = "\033[1;32m";   // Green/Bold
const std::string COMMENT = "\033[1;90m";   // Grey
const std::string SYMBOL  = "\033[1;34m";   // Blue/Bold

// --- QuarterLang Reserved Keywords (update as needed) ---
const std::unordered_set<std::string> KEYWORDS = {
    "star", "end", "val", "var", "say", "do", "loop", "to", "as", "if",
    "else", "match", "break", "continue", "fn", "dg", "type", "in", "not"
    // ... Add all your QuarterLang keywords here
};

class SyntaxHighlighter {
public:
    // Highlight one line of code
    std::string highlight(const std::string& line) {
        std::ostringstream result;
        size_t i = 0, n = line.size();

        while (i < n) {
            // Skip whitespace
            if (std::isspace(line[i])) {
                result << line[i];
                ++i;
                continue;
            }

            // --- Comments: '#' to end of line ---
            if (line[i] == '#') {
                result << COMMENT << line.substr(i) << RESET;
                break;
            }

            // --- Strings: double or single quote ---
            if (line[i] == '"' || line[i] == '\'') {
                char quote = line[i];
                size_t start = i++;
                while (i < n && line[i] != quote) {
                    if (line[i] == '\\' && i + 1 < n) i++; // skip escaped
                    i++;
                }
                if (i < n) i++; // skip closing quote
                result << STRING << line.substr(start, i - start) << RESET;
                continue;
            }

            // --- Numbers: integer/float ---
            if (std::isdigit(line[i])) {
                size_t start = i;
                while (i < n && (std::isdigit(line[i]) || line[i] == '.')) i++;
                result << NUMBER << line.substr(start, i - start) << RESET;
                continue;
            }

            // --- Identifiers / Keywords ---
            if (std::isalpha(line[i]) || line[i] == '_') {
                size_t start = i;
                while (i < n && (std::isalnum(line[i]) || line[i] == '_')) i++;
                std::string token = line.substr(start, i - start);
                if (KEYWORDS.count(token))
                    result << KEYWORD << token << RESET;
                else
                    result << IDENT << token << RESET;
                continue;
            }

            // --- Symbols: handle one or two char operators ---
            if (ispunct(line[i])) {
                // Try two-char (e.g. '==', '!=', etc.)
                if (i + 1 < n && ispunct(line[i+1])) {
                    result << SYMBOL << line.substr(i, 2) << RESET;
                    i += 2;
                } else {
                    result << SYMBOL << line[i] << RESET;
                    i++;
                }
                continue;
            }

            // Fallback: copy char
            result << line[i++];
        }
        return result.str();
    }

    // Highlight a multi-line source
    void highlightSource(const std::string& src) {
        std::istringstream iss(src);
        std::string line;
        while (std::getline(iss, line))
            std::cout << highlight(line) << std::endl;
    }
};

} // namespace QuarterLang

// === Demo: Run highlighter on some code ===
int main() {
    std::string code = R"(
star
  val x as 10
  say "Hello, Quarter!" # Output greeting
  loop to 5
    say x
  end
end
)";
    QuarterLang::SyntaxHighlighter hl;
    hl.highlightSource(code);
    return 0;
}

// QuarterLang Syntax Highlighter — C++
// Console/Terminal version using ANSI colors

#include <iostream>
#include <string>
#include <unordered_set>
#include <cctype>
#include <sstream>

namespace QuarterLang {

// --- ANSI Color Codes for terminal output ---
const std::string RESET  = "\033[0m";
const std::string KEYWORD = "\033[1;35m";   // Magenta/Bold
const std::string IDENT   = "\033[1;36m";   // Cyan/Bold
const std::string NUMBER  = "\033[1;33m";   // Yellow/Bold
const std::string STRING  = "\033[1;32m";   // Green/Bold
const std::string COMMENT = "\033[1;90m";   // Grey
const std::string SYMBOL  = "\033[1;34m";   // Blue/Bold

// --- QuarterLang Reserved Keywords (update as needed) ---
const std::unordered_set<std::string> KEYWORDS = {
    "star", "end", "val", "var", "say", "do", "loop", "to", "as", "if",
    "else", "match", "break", "continue", "fn", "dg", "type", "in", "not"
    // ... Add all your QuarterLang keywords here
};

class SyntaxHighlighter {
public:
    // Highlight one line of code
    std::string highlight(const std::string& line) {
        std::ostringstream result;
        size_t i = 0, n = line.size();

        while (i < n) {
            // Skip whitespace
            if (std::isspace(line[i])) {
                result << line[i];
                ++i;
                continue;
            }

            // --- Comments: '#' to end of line ---
            if (line[i] == '#') {
                result << COMMENT << line.substr(i) << RESET;
                break;
            }

            // --- Strings: double or single quote ---
            if (line[i] == '"' || line[i] == '\'') {
                char quote = line[i];
                size_t start = i++;
                while (i < n && line[i] != quote) {
                    if (line[i] == '\\' && i + 1 < n) i++; // skip escaped
                    i++;
                }
                if (i < n) i++; // skip closing quote
                result << STRING << line.substr(start, i - start) << RESET;
                continue;
            }

            // --- Numbers: integer/float ---
            if (std::isdigit(line[i])) {
                size_t start = i;
                while (i < n && (std::isdigit(line[i]) || line[i] == '.')) i++;
                result << NUMBER << line.substr(start, i - start) << RESET;
                continue;
            }

            // --- Identifiers / Keywords ---
            if (std::isalpha(line[i]) || line[i] == '_') {
                size_t start = i;
                while (i < n && (std::isalnum(line[i]) || line[i] == '_')) i++;
                std::string token = line.substr(start, i - start);
                if (KEYWORDS.count(token))
                    result << KEYWORD << token << RESET;
                else
                    result << IDENT << token << RESET;
                continue;
            }

            // --- Symbols: handle one or two char operators ---
            if (ispunct(line[i])) {
                // Try two-char (e.g. '==', '!=', etc.)
                if (i + 1 < n && ispunct(line[i+1])) {
                    result << SYMBOL << line.substr(i, 2) << RESET;
                    i += 2;
                } else {
                    result << SYMBOL << line[i] << RESET;
                    i++;
                }
                continue;
            }

            // Fallback: copy char
            result << line[i++];
        }
        return result.str();
    }

    // Highlight a multi-line source
    void highlightSource(const std::string& src) {
        std::istringstream iss(src);
        std::string line;
        while (std::getline(iss, line))
            std::cout << highlight(line) << std::endl;
    }
};

} // namespace QuarterLang

// === Demo: Run highlighter on some code ===
int main() {
    std::string code = R"(
star
  val x as 10
  say "Hello, Quarter!" # Output greeting
  loop to 5
    say x
  end
end
)";
    QuarterLang::SyntaxHighlighter hl;
    hl.highlightSource(code);
    return 0;
}

// quarter_debugger.h

#pragma once
#include <iostream>
#include <unordered_set>
#include <vector>
#include <string>
#include <memory>
#include <map>

// Forward declarations
class VM;

class QuarterDebugger {
public:
    QuarterDebugger(VM* vm);

    void repl(); // Main debug shell
    void setBreakpoint(int line);
    void clearBreakpoint(int line);
    void listBreakpoints();
    void continueExecution();
    void step();
    void inspectVars();
    void printStack();
    bool checkBreakpoint(int line);

    void notifyLine(int line); // Hook called by VM at each line

private:
    VM* vm_;
    std::unordered_set<int> breakpoints_;
    bool stepping_ = false;
    bool paused_ = false;
    int lastLine_ = -1;
};

#pragma once
#include <string>
#include <vector>

class QuarterFiler {
public:
    // Load file content as string
    static bool load(const std::string& filepath, std::string& outContent);

    // Save string content to file
    static bool save(const std::string& filepath, const std::string& content);

    // List all *.qr files in a directory (optionally recursive)
    static std::vector<std::string> listQuarterFiles(const std::string& directory, bool recursive = false);

    // Check if file exists
    static bool exists(const std::string& filepath);

    // Delete file
    static bool remove(const std::string& filepath);

private:
    // Utility: List files with given extension
    static void listFilesHelper(const std::string& directory, const std::string& ext, bool recursive, std::vector<std::string>& files);
};

#include "QuarterFiler.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

bool QuarterFiler::load(const std::string& filepath, std::string& outContent) {
    std::ifstream file(filepath, std::ios::in | std::ios::binary);
    if (!file) return false;
    outContent.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return true;
}

bool QuarterFiler::save(const std::string& filepath, const std::string& content) {
    std::ofstream file(filepath, std::ios::out | std::ios::binary);
    if (!file) return false;
    file.write(content.c_str(), content.size());
    return true;
}

std::vector<std::string> QuarterFiler::listQuarterFiles(const std::string& directory, bool recursive) {
    std::vector<std::string> files;
    listFilesHelper(directory, ".qr", recursive, files);
    return files;
}

bool QuarterFiler::exists(const std::string& filepath) {
    return fs::exists(filepath) && fs::is_regular_file(filepath);
}

bool QuarterFiler::remove(const std::string& filepath) {
    std::error_code ec;
    return fs::remove(filepath, ec);
}

void QuarterFiler::listFilesHelper(const std::string& directory, const std::string& ext, bool recursive, std::vector<std::string>& files) {
    if (!fs::exists(directory) || !fs::is_directory(directory)) return;
    if (recursive) {
        for (auto& entry : fs::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ext)
                files.push_back(entry.path().string());
        }
    } else {
        for (auto& entry : fs::directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ext)
                files.push_back(entry.path().string());
        }
    }
}

#include "QuarterFiler.h"
#include <iostream>

int main() {
    std::string code;
    if (QuarterFiler::load("example.qr", code))
        std::cout << "Loaded code:\n" << code << std::endl;
    else
        std::cout << "Failed to load file.\n";

    // Save example
    QuarterFiler::save("output.qr", "star\nsay 'Hello, Quarter!'\nend\n");

    // List all .qr files
    auto files = QuarterFiler::listQuarterFiles(".", false);
    for (const auto& f : files)
        std::cout << "Quarter file: " << f << std::endl;

    // Check existence
    std::cout << "Does 'output.qr' exist? " << (QuarterFiler::exists("output.qr") ? "Yes" : "No") << std::endl;

    // Delete file
    if (QuarterFiler::remove("output.qr"))
        std::cout << "File deleted.\n";
}

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <unordered_set>

const std::unordered_set<std::string> blockStarters = {
    "star", "loop", "if", "match", "else", "elif"
};
const std::unordered_set<std::string> blockEnders = {
    "end"
};

std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t");
    return s.substr(start, end - start + 1);
}

std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

// Simple tokenization (space split) for block detection.
std::string firstWord(const std::string& line) {
    auto trimmed = trim(line);
    auto pos = trimmed.find(' ');
    if (pos == std::string::npos) return trimmed;
    return trimmed.substr(0, pos);
}

// Main formatting logic
void formatQuarterLang(std::istream& in, std::ostream& out) {
    int indentLevel = 0;
    const int indentSpaces = 4;
    std::string line;
    while (std::getline(in, line)) {
        std::string trimmed = trim(line);

        if (trimmed.empty()) {
            out << std::endl;
            continue;
        }

        std::string fw = firstWord(trimmed);

        // Decrease indent before "end"
        if (blockEnders.count(fw)) {
            indentLevel = std::max(0, indentLevel - 1);
        }

        // Print indent
        out << std::string(indentLevel * indentSpaces, ' ');

        // Beautify spacing: operators, commas, parentheses
        std::string formatted = trimmed;
        formatted = std::regex_replace(formatted, std::regex(R"(\s*([=+\-*/<>])\s*)"), " $1 ");
        formatted = std::regex_replace(formatted, std::regex(R"(\s*,\s*)"), ", ");
        formatted = std::regex_replace(formatted, std::regex(R"(\(\s*)"), "(");
        formatted = std::regex_replace(formatted, std::regex(R"(\s*\))"), ")");

        out << formatted << std::endl;

        // Increase indent after block starters (except "else"—keep at same level)
        if (blockStarters.count(fw) && fw != "else" && fw != "elif") {
            indentLevel++;
        }
        // Special handling: indent after else/elif if not followed by 'end' directly
        if ((fw == "else" || fw == "elif")) {
            indentLevel++;
        }
        // If "end" follows "else" or "elif" directly, correct indent
        if (fw == "end" && (indentLevel > 0)) {
            indentLevel--;
        }
    }
}

// Entry point: format from file or stdin
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "QuarterLang Formatter\nUsage: " << argv[0] << " <inputfile>\n";
        return 1;
    }

    std::ifstream infile(argv[1]);
    if (!infile) {
        std::cerr << "Failed to open input file.\n";
        return 1;
    }

    formatQuarterLang(infile, std::cout);

    return 0;
}

// QuarterLang Renderer — Core Module
// [Transforms AST nodes into formatted source or intermediate code]
// (C) 2025 Violet Aura Creations, All Rights Reserved

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <map>

// === AST Node Kinds ===
enum class ASTNodeKind {
    Program,
    Statement,
    Block,
    VariableDecl,
    Assignment,
    Literal,
    Identifier,
    Expression,
    FunctionDecl,
    FunctionCall,
    If,
    Loop,
    Say,
    // ... extend as needed
};

// === AST Node Base ===
struct ASTNode {
    ASTNodeKind kind;
    std::vector<std::shared_ptr<ASTNode>> children;

    virtual ~ASTNode() = default;
};

// --- Example AST Node Subtypes ---

struct ASTLiteral : ASTNode {
    std::string value;
    ASTLiteral(const std::string& val) : value(val) { kind = ASTNodeKind::Literal; }
};

struct ASTIdentifier : ASTNode {
    std::string name;
    ASTIdentifier(const std::string& n) : name(n) { kind = ASTNodeKind::Identifier; }
};

struct ASTVariableDecl : ASTNode {
    std::string name;
    std::string type;
    std::shared_ptr<ASTNode> initValue;
    ASTVariableDecl(const std::string& n, const std::string& t, std::shared_ptr<ASTNode> v)
        : name(n), type(t), initValue(v) { kind = ASTNodeKind::VariableDecl; }
};

struct ASTBlock : ASTNode {
    ASTBlock() { kind = ASTNodeKind::Block; }
};

// ... add more as needed (Assignment, If, FunctionDecl, etc.)

// === Renderer Class ===
class QuarterRenderer {
public:
    QuarterRenderer() {}

    std::string render(const std::shared_ptr<ASTNode>& node, int indent = 0) {
        std::ostringstream oss;
        renderNode(node, oss, indent);
        return oss.str();
    }

private:
    void renderNode(const std::shared_ptr<ASTNode>& node, std::ostringstream& oss, int indent) {
        if (!node) return;

        switch (node->kind) {
            case ASTNodeKind::Program:
            case ASTNodeKind::Block:
                for (const auto& child : node->children)
                    renderNode(child, oss, indent);
                break;

            case ASTNodeKind::VariableDecl: {
                auto decl = static_cast<ASTVariableDecl*>(node.get());
                oss << std::string(indent, ' ')
                    << "val " << decl->type << " " << decl->name;
                if (decl->initValue) {
                    oss << " = ";
                    renderNode(decl->initValue, oss, 0);
                }
                oss << ";\n";
                break;
            }

            case ASTNodeKind::Literal: {
                auto lit = static_cast<ASTLiteral*>(node.get());
                oss << lit->value;
                break;
            }

            case ASTNodeKind::Identifier: {
                auto id = static_cast<ASTIdentifier*>(node.get());
                oss << id->name;
                break;
            }

            // --- Add other node types as needed ---
            // Assignment, FunctionDecl, If, Loop, Say, etc.

            default:
                oss << "[UnknownNode]";
                break;
        }
    }
};

//// ==============================
// === EXAMPLE USAGE DEMO BELOW ===
// ===============================

int main() {
    // Example: val int x = 42;
    auto lit42 = std::make_shared<ASTLiteral>("42");
    auto varX = std::make_shared<ASTVariableDecl>("x", "int", lit42);

    // Put in a block/program node
    auto block = std::make_shared<ASTBlock>();
    block->children.push_back(varX);

    QuarterRenderer renderer;
    std::string output = renderer.render(block);

    std::cout << "=== QuarterLang Rendered Output ===\n";
    std::cout << output;

    return 0;
}

#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <stdexcept>

struct QuarterLibrary {
    std::string name;                 // "math", "io", etc.
    std::string sourceCode;           // Original QuarterLang source (for dynamic import)
    std::vector<std::string> dependencies; // Names of libraries this imports
    bool loaded = false;              // If the lib has been loaded (imported and parsed)
    // Optional: void* bytecode, or compiled function map
};

class QuarterLibrarySystem {
public:
    // Register a library with its source code and dependencies
    void registerLibrary(const std::string& name, const std::string& sourceCode,
                         const std::vector<std::string>& dependencies = {});
    // Load (and resolve dependencies) for a given library
    void loadLibrary(const std::string& name);

    // Retrieve library by name
    const QuarterLibrary& getLibrary(const std::string& name) const;

    // List all registered libraries
    std::vector<std::string> listLibraries() const;

    // Check if a library exists
    bool hasLibrary(const std::string& name) const;

private:
    std::unordered_map<std::string, QuarterLibrary> libraries;
    std::unordered_set<std::string> loadingStack; // for cycle detection
};

#include "QuarterLibrarySystem.h"

void QuarterLibrarySystem::registerLibrary(
    const std::string& name,
    const std::string& sourceCode,
    const std::vector<std::string>& dependencies
) {
    if (libraries.count(name)) {
        throw std::runtime_error("Library '" + name + "' already registered.");
    }
    libraries[name] = QuarterLibrary{name, sourceCode, dependencies, false};
}

void QuarterLibrarySystem::loadLibrary(const std::string& name) {
    if (!libraries.count(name)) {
        throw std::runtime_error("Library '" + name + "' not found.");
    }
    // Prevent cyclic imports
    if (loadingStack.count(name)) {
        throw std::runtime_error("Cyclic import detected for library '" + name + "'");
    }
    // Already loaded
    if (libraries[name].loaded) return;

    loadingStack.insert(name);
    // First, load dependencies
    for (const auto& dep : libraries[name].dependencies) {
        loadLibrary(dep);
    }
    // Here, you would normally parse or compile the sourceCode, etc.
    libraries[name].loaded = true;
    loadingStack.erase(name);
}

const QuarterLibrary& QuarterLibrarySystem::getLibrary(const std::string& name) const {
    auto it = libraries.find(name);
    if (it == libraries.end())
        throw std::runtime_error("Library '" + name + "' not found.");
    return it->second;
}

std::vector<std::string> QuarterLibrarySystem::listLibraries() const {
    std::vector<std::string> names;
    for (const auto& kv : libraries) names.push_back(kv.first);
    return names;
}

bool QuarterLibrarySystem::hasLibrary(const std::string& name) const {
    return libraries.count(name) > 0;
}

QuarterLibrarySystem libsys;

libsys.registerLibrary("math", "star val pi = 3.14 end", {});
libsys.registerLibrary("utils", "star say 'Hello!' end", {"math"});

libsys.loadLibrary("utils");

for (auto& name : libsys.listLibraries()) {
    const auto& lib = libsys.getLibrary(name);
    std::cout << "Loaded: " << name << " [deps: ";
    for (const auto& d : lib.dependencies) std::cout << d << " ";
    std::cout << "]\n";
}

// QuarterLang Injector — C++ Example
// Author: Violet
// Purpose: Injects code snippets/hooks into QuarterLang source code for pre-processing

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// --- Injection Points ---
const std::string INJECT_BEFORE_STAR = "// [Injected] :: Entering new scope\n";
const std::string INJECT_AFTER_STAR  = "say \"[Injected] Scope started\";\n";
const std::string INJECT_BEFORE_END  = "say \"[Injected] Scope ending\";\n";
const std::string INJECT_AFTER_END   = "// [Injected] :: Scope closed\n";

// --- Utility: Read file into string ---
std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) throw std::runtime_error("Could not open file: " + filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// --- Utility: Write string to file ---
void writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file) throw std::runtime_error("Could not write to file: " + filename);
    file << content;
}

// --- Core Injection Function ---
std::string injectQuarterLang(const std::string& src) {
    std::istringstream iss(src);
    std::ostringstream oss;
    std::string line;
    bool inScope = false;

    while (std::getline(iss, line)) {
        // Detect 'star' and 'end' keywords for injection points
        std::string trimmed = line;
        // Trim leading spaces
        trimmed.erase(0, trimmed.find_first_not_of(" \t\r\n"));

        if (trimmed == "star") {
            oss << INJECT_BEFORE_STAR;
            oss << line << "\n";
            oss << INJECT_AFTER_STAR;
            inScope = true;
        }
        else if (trimmed == "end") {
            oss << INJECT_BEFORE_END;
            oss << line << "\n";
            oss << INJECT_AFTER_END;
            inScope = false;
        }
        else {
            oss << line << "\n";
        }
    }
    return oss.str();
}

// --- Main Example Usage ---
int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: injector <input.quarter> <output.quarter>\n";
        return 1;
    }

    try {
        std::string input = readFile(argv[1]);
        std::string injected = injectQuarterLang(input);
        writeFile(argv[2], injected);
        std::cout << "Injection complete. Output written to " << argv[2] << "\n";
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 2;
    }
    return 0;
}

// QuarterLang Seeder — C++ Implementation
// Generates starter QuarterLang scripts, examples, or demo projects.

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

class QuarterLangSeeder {
public:
    // Seed directory path, can be customized.
    explicit QuarterLangSeeder(const std::string& outDir = "quarter_seed")
        : outputDir(outDir) {}

    // Main entry: seeds a batch of example files.
    void seed(size_t numExamples = 3) {
        std::filesystem::create_directories(outputDir);
        for (size_t i = 1; i <= numExamples; ++i) {
            std::string filename = outputDir + "/example" + std::to_string(i) + ".quarter";
            std::ofstream out(filename);
            if (out.is_open()) {
                out << getExample(i);
                out.close();
                std::cout << "Seeded: " << filename << std::endl;
            } else {
                std::cerr << "Error creating file: " << filename << std::endl;
            }
        }
    }

    // Returns a string with a seeded QuarterLang program.
    std::string getExample(size_t which) {
        switch (which) {
        case 1:
            return R"(star
    val x = 7
    val y = 3
    val sum = x + y
    say "The sum is: " sum
end
)";
        case 2:
            return R"(star
    var counter = 0
    loop 5
        say "Counter:" counter
        counter = counter + 1
    end
end
)";
        case 3:
            return R"(star
    val greet = "QuarterLang Seeder!"
    say greet
    if greet == "QuarterLang Seeder!"
        say "Greeting matched!"
    end
end
)";
        default:
            return R"(star
    say "This is a default QuarterLang seed example."
end
)";
        }
    }

private:
    std::string outputDir;
};

// --- USAGE EXAMPLE ---
// int main() {
//     QuarterLangSeeder seeder("quarter_seed"); // Directory name
//     seeder.seed(3); // Number of examples to generate
//     return 0;
// }

// === QuarterLang Encapsulation in C++ ===
// "star" ... "end" => explicit class boundary
// Public interface exposes only what is meant to be shared

#include <iostream>
#include <string>

// star encapsulation
class Encapsulator {
private:
    // Internal state is private — not directly accessible
    int secret_value;
    std::string internal_note;

public:
    // star: constructor
    Encapsulator(int val, const std::string& note) : secret_value(val), internal_note(note) {}
    // end: constructor

    // star: public API

    // Set internal value (explicit mutation)
    void set_value(int new_value) {
        secret_value = new_value;
    }

    // Get internal value (read-only access)
    int get_value() const {
        return secret_value;
    }

    // Set internal note
    void set_note(const std::string& note) {
        internal_note = note;
    }

    // Public action using encapsulated data
    void announce() const {
        std::cout << "Current value: " << secret_value
                  << ", note: " << internal_note << std::endl;
    }

    // end: public API
};
// end encapsulation

// === Example usage ===
int main() {
    // star: usage
    Encapsulator qbox(42, "Initial quarter");
    qbox.announce();

    qbox.set_value(99);
    qbox.set_note("Updated for runtime!");
    qbox.announce();
    // end: usage

    // Output:
    // Current value: 42, note: Initial quarter
    // Current value: 99, note: Updated for runtime!
    return 0;
}

// QuarterLang Scoper - C++ Implementation
// =======================================
// Manages nested scopes for variables and values.

#include <iostream>
#include <unordered_map>
#include <vector>
#include <optional>
#include <string>

// Value struct for demonstration; adapt as needed for full language type support.
struct QValue {
    std::string type;   // e.g., "int", "text"
    std::string value;  // actual value (or use variant for richer type support)
};

class QuarterScoper {
    using Scope = std::unordered_map<std::string, QValue>;
    std::vector<Scope> scopes;

public:
    QuarterScoper() {
        // Start with a global (outermost) scope.
        scopes.push_back({});
    }

    // Enter a new scope (on 'star')
    void push_scope() {
        scopes.push_back({});
    }

    // Exit the current scope (on 'end')
    void pop_scope() {
        if (scopes.size() > 1) { // Never pop the global scope
            scopes.pop_back();
        } else {
            std::cerr << "[Scoper] Warning: Attempt to pop global scope ignored." << std::endl;
        }
    }

    // Define or overwrite a variable in the current scope
    void define(const std::string& name, const QValue& val) {
        scopes.back()[name] = val;
    }

    // Find a variable in the scope chain (innermost to outermost)
    std::optional<QValue> lookup(const std::string& name) const {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            auto found = it->find(name);
            if (found != it->end()) {
                return found->second;
            }
        }
        return std::nullopt; // Not found
    }

    // Assign a variable (will find and assign in the nearest scope where it exists, else defines in current scope)
    void assign(const std::string& name, const QValue& val) {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            auto found = it->find(name);
            if (found != it->end()) {
                found->second = val;
                return;
            }
        }
        // Not found in any scope: define in current scope
        scopes.back()[name] = val;
    }

    // Debug: print all scopes (innermost first)
    void debug_print() const {
        std::cout << "=== QuarterLang Scopes (innermost first) ===\n";
        int level = scopes.size();
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it, --level) {
            std::cout << "Scope Level " << level << ":\n";
            for (const auto& [k, v] : *it) {
                std::cout << "  " << k << " = (" << v.type << ") " << v.value << "\n";
            }
        }
    }
};

// --- Example Usage ---
/*
int main() {
    QuarterScoper scoper;
    scoper.define("x", {"int", "5"});
    scoper.push_scope();
    scoper.define("x", {"int", "10"});
    scoper.define("y", {"text", "hello"});
    scoper.debug_print();

    auto val = scoper.lookup("x");
    if (val) std::cout << "Found x: " << val->value << std::endl;

    scoper.pop_scope();
    scoper.debug_print();

    return 0;
}
*/

// QuarterLang Memory Handler — Core Implementation

#include <iostream>
#include <unordered_map>
#include <string>
#include <variant>
#include <stdexcept>
#include <memory>
#include <stack>

enum class QType { QInt, QText, QDG, QUnknown };

struct QValue {
    QType type;
    std::variant<int, std::string, double> value;
    bool immutable;

    QValue() : type(QType::QUnknown), value(0), immutable(false) {}
    QValue(int v, bool im=false) : type(QType::QInt), value(v), immutable(im) {}
    QValue(const std::string& v, bool im=false) : type(QType::QText), value(v), immutable(im) {}
    QValue(double v, bool im=false) : type(QType::QDG), value(v), immutable(im) {}
};

class QuarterMemoryHandler {
public:
    // Scope-aware: use stack for block-level vars (star ... end)
    void enterScope() {
        memoryScopes.emplace();
    }

    void exitScope() {
        if (!memoryScopes.empty()) {
            memoryScopes.pop();
        }
    }

    // Allocate a variable or constant
    void allocate(const std::string& name, const QValue& val) {
        if (memoryScopes.empty()) enterScope();
        if (memoryScopes.top().count(name)) {
            throw std::runtime_error("Variable '" + name + "' already exists in this scope.");
        }
        memoryScopes.top()[name] = val;
    }

    // Assign to existing variable
    void assign(const std::string& name, const QValue& val) {
        for (auto it = memoryScopes.rbegin(); it != memoryScopes.rend(); ++it) {
            if (it->count(name)) {
                if ((*it)[name].immutable)
                    throw std::runtime_error("Cannot assign to immutable (val) '" + name + "'");
                (*it)[name] = val;
                return;
            }
        }
        throw std::runtime_error("Variable '" + name + "' not found");
    }

    // Retrieve a value
    QValue get(const std::string& name) const {
        for (auto it = memoryScopes.crbegin(); it != memoryScopes.crend(); ++it) {
            if (it->count(name)) return it->at(name);
        }
        throw std::runtime_error("Variable '" + name + "' not found");
    }

    // Deallocate a variable from current scope
    void deallocate(const std::string& name) {
        if (!memoryScopes.empty()) {
            memoryScopes.top().erase(name);
        }
    }

    // Debug print all memory slots (current scope)
    void debugPrint() const {
        std::cout << "Memory Handler: Current Scope Vars:\n";
        if (memoryScopes.empty()) return;
        for (const auto& [k, v] : memoryScopes.top()) {
            std::cout << "  " << k << " = ";
            switch (v.type) {
                case QType::QInt: std::cout << std::get<int>(v.value); break;
                case QType::QText: std::cout << std::get<std::string>(v.value); break;
                case QType::QDG: std::cout << std::get<double>(v.value); break;
                default: std::cout << "<?>"; break;
            }
            std::cout << (v.immutable ? " (val)" : " (var)") << "\n";
        }
    }

private:
    // Stack of variable tables for block scoping
    std::stack<std::unordered_map<std::string, QValue>> memoryScopes;
};


// === Example Usage ===
int main() {
    QuarterMemoryHandler mem;
    mem.enterScope(); // star

    mem.allocate("x", QValue(42, true));      // val x = 42
    mem.allocate("msg", QValue(std::string("hello"))); // var msg = "hello"
    mem.allocate("pi", QValue(3.14, true));   // val pi = 3.14

    mem.debugPrint();

    try {
        mem.assign("msg", QValue(std::string("world")));
        // mem.assign("x", QValue(999)); // This would throw (immutable)
        std::cout << "msg after assign: " << std::get<std::string>(mem.get("msg").value) << "\n";
    } catch (std::exception& e) {
        std::cerr << e.what() << "\n";
    }

    mem.exitScope(); // end

    return 0;
}

// QuarterLang: Range Adjuster — C++ Implementation
// -----------------------------------------------
//  * Clamp, Wrap, and Scale any value to a fixed [min, max] interval.
//  * Can be used for variable assignment, index validation, etc.

#include <iostream>
#include <algorithm> // for std::min, std::max

class RangeAdjuster {
public:
    // Constructor — define the initial range.
    RangeAdjuster(double minValue, double maxValue)
        : minVal(minValue), maxVal(maxValue) {
        if (minVal > maxVal) std::swap(minVal, maxVal);
    }

    // Set or update the range.
    void setRange(double minValue, double maxValue) {
        minVal = std::min(minValue, maxValue);
        maxVal = std::max(minValue, maxValue);
    }

    // Clamp: restricts value to [minVal, maxVal].
    double clamp(double value) const {
        return std::max(minVal, std::min(value, maxVal));
    }

    // Wrap: cycles value within [minVal, maxVal] (modulo behavior).
    double wrap(double value) const {
        double range = maxVal - minVal + 1;
        if (range <= 0) return minVal;
        double wrapped = std::fmod(value - minVal, range);
        if (wrapped < 0) wrapped += range;
        return minVal + wrapped;
    }

    // Scale: rescales value from [inMin, inMax] to [minVal, maxVal]
    double scale(double value, double inMin, double inMax) const {
        if (inMin == inMax) return minVal;
        double norm = (value - inMin) / (inMax - inMin);
        return minVal + norm * (maxVal - minVal);
    }

    // (Optional) Snap: rounds to nearest step within range.
    double snap(double value, double step) const {
        double clamped = clamp(value);
        double snapped = minVal + std::round((clamped - minVal) / step) * step;
        return clamp(snapped);
    }

private:
    double minVal, maxVal;
};

// --------- DEMO USAGE ----------
int main() {
    RangeAdjuster r(10, 20);

    double x1 = 25;
    double x2 = 7;
    double x3 = 15;

    std::cout << "Clamp(25): " << r.clamp(x1) << std::endl;   // Output: 20
    std::cout << "Clamp(7): " << r.clamp(x2) << std::endl;    // Output: 10
    std::cout << "Clamp(15): " << r.clamp(x3) << std::endl;   // Output: 15

    std::cout << "Wrap(25): " << r.wrap(x1) << std::endl;     // Output: 15
    std::cout << "Wrap(7): " << r.wrap(x2) << std::endl;      // Output: 18

    std::cout << "Scale(5, 0, 10): " << r.scale(5, 0, 10) << std::endl; // Output: 15

    std::cout << "Snap(16.7, 1.5): " << r.snap(16.7, 1.5) << std::endl; // Output: 16.0

    return 0;
}

// ===== QuarterLang "Garbage Handler" — Deterministic Memory Collector =====
//   Inspired by Quarter's "star...end" explicit scoping and zero-leak ethos.

#include <iostream>
#include <vector>
#include <unordered_set>

class GarbageHandler {
public:
    // Register a pointer to be garbage-collected.
    template <typename T>
    T* track(T* ptr) {
        allocations.insert(static_cast<void*>(ptr));
        return ptr;
    }

    // Remove pointer from tracking (if manually deleted).
    template <typename T>
    void untrack(T* ptr) {
        allocations.erase(static_cast<void*>(ptr));
    }

    // Mass-deallocate all tracked pointers (e.g., at end of "star...end" block)
    void cleanup() {
        for (void* ptr : allocations) {
            deletePtr(ptr); // Deallocate appropriately
        }
        allocations.clear();
    }

    // Destructor cleans up any remaining tracked pointers.
    ~GarbageHandler() {
        cleanup();
    }

private:
    std::unordered_set<void*> allocations;

    // Delete as T (assumes pointers are new-allocated, not arrays).
    static void deletePtr(void* ptr) {
        // In a more advanced system, you'd track type info for safe deletion.
        // For basic demo: treat as 'char*'
        delete static_cast<char*>(ptr);
    }
};

// ===== Usage Example =====
struct MyObj {
    int x;
    MyObj(int x) : x(x) { std::cout << "[ALLOC] MyObj " << x << std::endl; }
    ~MyObj() { std::cout << "[FREE ] MyObj " << x << std::endl; }
};

int main() {
    GarbageHandler ghandler;

    // Allocate and register objects
    auto* a = ghandler.track(new MyObj(7));
    auto* b = ghandler.track(new MyObj(42));

    // You can untrack and manually delete if needed:
    ghandler.untrack(a);
    delete a;

    // Remaining allocations are auto-freed on cleanup or handler destruction.
    ghandler.cleanup();

    // All memory now freed, even if user forgot some deletes.
    return 0;
}

// --- QuarterLang-Inspired Tracker ---
// Tracks events, variable changes, and checkpoints.

#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>

class Tracker {
public:
    // Track changes in variables by name
    void trackVariable(const std::string& varName, const std::string& value) {
        variableHistory[varName].push_back(value);
        std::cout << "[TRACK] " << varName << " updated to: " << value << "\n";
    }

    // Track a named event (with optional details)
    void trackEvent(const std::string& eventName, const std::string& details = "") {
        events.push_back(eventName + (details.empty() ? "" : (": " + details)));
        std::cout << "[EVENT] " << eventName;
        if (!details.empty()) std::cout << " (" << details << ")";
        std::cout << "\n";
    }

    // Get full history of a variable
    void printVariableHistory(const std::string& varName) {
        std::cout << "[HISTORY] " << varName << ": ";
        if (variableHistory.count(varName)) {
            for (const auto& v : variableHistory[varName])
                std::cout << v << " -> ";
            std::cout << "END\n";
        } else {
            std::cout << "No history found.\n";
        }
    }

    // Print all events
    void printEvents() {
        std::cout << "[EVENT LOG]\n";
        for (const auto& e : events) std::cout << "- " << e << "\n";
    }

private:
    std::unordered_map<std::string, std::vector<std::string>> variableHistory;
    std::vector<std::string> events;
};

// --- QuarterLang-Inspired Tracer ---
// Traces function calls and code execution paths.

#include <iostream>
#include <string>
#include <vector>

class Tracer {
public:
    // Log function entry
    void enterFunction(const std::string& functionName) {
        callStack.push_back(functionName);
        std::cout << "[TRACE] Entering: " << functionName << "\n";
    }

    // Log function exit
    void exitFunction(const std::string& functionName) {
        if (!callStack.empty() && callStack.back() == functionName) {
            std::cout << "[TRACE] Exiting: " << functionName << "\n";
            callStack.pop_back();
        }
    }

    // Print current call stack
    void printStack() {
        std::cout << "[CALL STACK] ";
        for (const auto& fn : callStack) std::cout << fn << " > ";
        std::cout << "END\n";
    }

private:
    std::vector<std::string> callStack;
};

// Macro for automatic function tracing (for ease of use)
#define TRACE_FUNCTION(tracer) FunctionTracer __functionTracer__(tracer, __func__)
class FunctionTracer {
    Tracer& tracer;
    std::string funcName;
public:
    FunctionTracer(Tracer& t, const std::string& name) : tracer(t), funcName(name) {
        tracer.enterFunction(funcName);
    }
    ~FunctionTracer() {
        tracer.exitFunction(funcName);
    }
};

// QuarterLang Conceptulizer — C++ Implementation
// This component builds semantic concepts from AST nodes, e.g., functions, variables, blocks.

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

// --- AST Node Structure (Simplified) ---
enum class ASTNodeType {
    Program,        // Root
    Statement,      // General statement
    VariableDecl,   // Variable declaration
    FunctionDecl,   // Function definition
    Block,          // Block/scope
    Call,           // Function call
    Assignment,     // Assignment
    Expression,     // Expression
    // ... more as needed
};

struct ASTNode {
    ASTNodeType type;
    std::string value;                   // Identifier, literal, etc.
    std::vector<std::shared_ptr<ASTNode>> children;
    int line;

    ASTNode(ASTNodeType t, const std::string& v = "", int l = 0)
        : type(t), value(v), line(l) {}
};

// --- QuarterLang Concept Structure ---
struct Concept {
    std::string name;
    std::string kind; // "function", "variable", "block", etc.
    int line;
    std::vector<std::string> dependencies;  // Used for further linkage/analysis

    Concept(const std::string& n, const std::string& k, int l)
        : name(n), kind(k), line(l) {}
};

// --- Conceptulizer Class ---
class Conceptulizer {
public:
    explicit Conceptulizer(const std::shared_ptr<ASTNode>& root) : astRoot(root) {}

    // Main entry: builds the concept map from AST
    void buildConcepts() {
        concepts.clear();
        traverse(astRoot);
    }

    // Access concepts (e.g. for later IR/codegen)
    const std::vector<Concept>& getConcepts() const { return concepts; }

    // Debug: Print all discovered concepts
    void debugPrint() const {
        std::cout << "[Conceptulizer] Discovered Concepts:\n";
        for (const auto& c : concepts) {
            std::cout << " - " << c.kind << ": " << c.name << " (line " << c.line << ")\n";
        }
    }

private:
    std::shared_ptr<ASTNode> astRoot;
    std::vector<Concept> concepts;

    // Recursive traversal & semantic grouping
    void traverse(const std::shared_ptr<ASTNode>& node) {
        if (!node) return;

        switch (node->type) {
        case ASTNodeType::VariableDecl:
            concepts.emplace_back(node->value, "variable", node->line);
            break;
        case ASTNodeType::FunctionDecl:
            concepts.emplace_back(node->value, "function", node->line);
            break;
        case ASTNodeType::Block:
            concepts.emplace_back("block@" + std::to_string(node->line), "block", node->line);
            break;
        // You can expand with more concept types as QuarterLang grows!
        default:
            break;
        }

        for (const auto& child : node->children) {
            traverse(child);
        }
    }
};

//// ======= EXAMPLE USAGE =======

int main() {
    // Example AST for: star fun main end
    auto root = std::make_shared<ASTNode>(ASTNodeType::Program);
    auto mainFunc = std::make_shared<ASTNode>(ASTNodeType::FunctionDecl, "main", 1);
    root->children.push_back(mainFunc);

    // Add a variable declaration to main
    auto var = std::make_shared<ASTNode>(ASTNodeType::VariableDecl, "score", 2);
    mainFunc->children.push_back(var);

    // Build and print concepts
    Conceptulizer conceptulizer(root);
    conceptulizer.buildConcepts();
    conceptulizer.debugPrint();

    return 0;
}

// === QuarterLang Configuration Manager ===
// "Clarity in every setting. Consistency in every run."

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <sstream>

class QuarterConfigManager {
private:
    std::unordered_map<std::string, std::string> configMap;
    std::string configFile;

    void parseLine(const std::string& line) {
        // No comments, no ambiguity, only explicit key:value
        size_t delimPos = line.find(':');
        if (delimPos != std::string::npos) {
            std::string key = line.substr(0, delimPos);
            std::string value = line.substr(delimPos + 1);
            // Strip whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            configMap[key] = value;
        }
    }

public:
    QuarterConfigManager(const std::string& filename)
        : configFile(filename) {
        load();
    }

    void load() {
        configMap.clear();
        std::ifstream inFile(configFile);
        std::string line;
        while (std::getline(inFile, line)) {
            if (!line.empty() && line[0] != '#') // skip comments
                parseLine(line);
        }
    }

    void save() const {
        std::ofstream outFile(configFile);
        for (const auto& [key, value] : configMap) {
            outFile << key << ": " << value << std::endl;
        }
    }

    std::string get(const std::string& key, const std::string& def = "") const {
        auto it = configMap.find(key);
        return (it != configMap.end()) ? it->second : def;
    }

    void set(const std::string& key, const std::string& value) {
        configMap[key] = value;
    }

    bool has(const std::string& key) const {
        return configMap.find(key) != configMap.end();
    }

    void remove(const std::string& key) {
        configMap.erase(key);
    }

    void printAll() const {
        std::cout << "=== QuarterLang Config ===" << std::endl;
        for (const auto& [key, value] : configMap) {
            std::cout << key << ": " << value << std::endl;
        }
    }
};

// === Demo/Usage ===
int main() {
    QuarterConfigManager config("quarter.cfg");

    // Set initial values
    config.set("runmode", "debug");
    config.set("max_threads", "4");
    config.set("optimize", "true");
    config.save();

    // Reload, fetch, print
    config.load();
    config.printAll();

    // Update value, save again
    config.set("runmode", "release");
    config.save();

    std::cout << "Mode: " << config.get("runmode") << std::endl;
    std::cout << "Threads: " << config.get("max_threads", "1") << std::endl;
    return 0;
}

// QuarterLang Manipulator System — C++
// Manipulates values, AST nodes, or custom structures

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <variant>
#include <map>

// --- Basic Value Type ---
struct QValue {
    enum class Type { Int, Text, Bool, None } type;
    std::variant<int, std::string, bool> value;

    QValue() : type(Type::None) {}
    QValue(int v) : type(Type::Int), value(v) {}
    QValue(const std::string& v) : type(Type::Text), value(v) {}
    QValue(bool v) : type(Type::Bool), value(v) {}
};

// --- AST Node (Minimal Example) ---
struct QASTNode {
    std::string nodeType;
    std::vector<std::shared_ptr<QASTNode>> children;
    QValue value;

    QASTNode(const std::string& type) : nodeType(type) {}
};

// --- Manipulator Interface ---
class Manipulator {
public:
    // Manipulate a QValue (e.g., increment, uppercase, invert)
    virtual QValue manipulateValue(const QValue& input) = 0;

    // Manipulate an AST node (e.g., fold, transform)
    virtual std::shared_ptr<QASTNode> manipulateNode(const std::shared_ptr<QASTNode>& node) = 0;

    virtual ~Manipulator() = default;
};

// --- Example Manipulator: Incrementer ---
class IncrementManipulator : public Manipulator {
public:
    QValue manipulateValue(const QValue& input) override {
        if (input.type == QValue::Type::Int)
            return QValue(std::get<int>(input.value) + 1);
        return input;
    }

    std::shared_ptr<QASTNode> manipulateNode(const std::shared_ptr<QASTNode>& node) override {
        // Recursively increment all int values in subtree
        auto newNode = std::make_shared<QASTNode>(node->nodeType);
        newNode->value = manipulateValue(node->value);
        for (const auto& child : node->children)
            newNode->children.push_back(manipulateNode(child));
        return newNode;
    }
};

// --- Example Manipulator: ToUppercase ---
class UppercaseManipulator : public Manipulator {
public:
    QValue manipulateValue(const QValue& input) override {
        if (input.type == QValue::Type::Text) {
            std::string up = std::get<std::string>(input.value);
            for (auto& c : up) c = std::toupper(c);
            return QValue(up);
        }
        return input;
    }
    std::shared_ptr<QASTNode> manipulateNode(const std::shared_ptr<QASTNode>& node) override {
        auto newNode = std::make_shared<QASTNode>(node->nodeType);
        newNode->value = manipulateValue(node->value);
        for (const auto& child : node->children)
            newNode->children.push_back(manipulateNode(child));
        return newNode;
    }
};

// --- Manipulator Registry (Optional) ---
class ManipulatorRegistry {
    std::map<std::string, std::shared_ptr<Manipulator>> manipulators;
public:
    void registerManipulator(const std::string& name, std::shared_ptr<Manipulator> manip) {
        manipulators[name] = manip;
    }
    std::shared_ptr<Manipulator> get(const std::string& name) {
        return manipulators.count(name) ? manipulators[name] : nullptr;
    }
};

//// --- Example Usage ---
/*
int main() {
    ManipulatorRegistry registry;
    registry.registerManipulator("inc", std::make_shared<IncrementManipulator>());
    registry.registerManipulator("upper", std::make_shared<UppercaseManipulator>());

    QValue val(10);
    QValue txt("quarterlang");

    auto inc = registry.get("inc");
    auto upper = registry.get("upper");

    std::cout << "Incremented: " << std::get<int>(inc->manipulateValue(val).value) << std::endl;
    std::cout << "Uppercase: " << std::get<std::string>(upper->manipulateValue(txt).value) << std::endl;
}
*/

// TransformAgent.hpp
#pragma once
#include <string>
#include <functional>
#include <map>
#include <vector>
#include <memory>
#include <stdexcept>

// -- Demo: Sample AST Node
struct QNode {
    std::string type;
    std::string value;
    std::vector<std::shared_ptr<QNode>> children;
};

enum class TransformDirection {
    Forward,
    Backward,
    Optimize,
    Minify,
    Expand,
    Custom // can add more directions as needed
};

class TransformAgent {
public:
    using TransformFunc = std::function<std::shared_ptr<QNode>(const std::shared_ptr<QNode>&)>;

    // Register a transformation for a direction
    void registerTransform(TransformDirection dir, TransformFunc func) {
        transforms[dir] = func;
    }

    // Transform in a specific direction
    std::shared_ptr<QNode> transform(const std::shared_ptr<QNode>& node, TransformDirection dir) const {
        auto it = transforms.find(dir);
        if (it != transforms.end()) {
            return it->second(node);
        }
        throw std::runtime_error("No transform registered for this direction.");
    }

    // List available directions
    std::vector<TransformDirection> availableDirections() const {
        std::vector<TransformDirection> dirs;
        for (const auto& kv : transforms) dirs.push_back(kv.first);
        return dirs;
    }

private:
    std::map<TransformDirection, TransformFunc> transforms;
};

///////////////////////
// Example Usage
///////////////////////

#include <iostream>

std::shared_ptr<QNode> forwardTransform(const std::shared_ptr<QNode>& node) {
    // Demo: uppercase value
    auto n = std::make_shared<QNode>(*node);
    for (char& c : n->value) c = toupper(c);
    return n;
}

std::shared_ptr<QNode> backwardTransform(const std::shared_ptr<QNode>& node) {
    // Demo: lowercase value
    auto n = std::make_shared<QNode>(*node);
    for (char& c : n->value) c = tolower(c);
    return n;
}

int main() {
    // Create a sample node
    auto node = std::make_shared<QNode>();
    node->type = "identifier";
    node->value = "QuarterLang";

    TransformAgent agent;
    agent.registerTransform(TransformDirection::Forward, forwardTransform);
    agent.registerTransform(TransformDirection::Backward, backwardTransform);

    // Forward transformation (UPPERCASE)
    auto fwd = agent.transform(node, TransformDirection::Forward);
    std::cout << "Forward: " << fwd->value << std::endl;

    // Backward transformation (lowercase)
    auto bwd = agent.transform(node, TransformDirection::Backward);
    std::cout << "Backward: " << bwd->value << std::endl;

    // List available directions
    for (auto dir : agent.availableDirections()) {
        std::cout << "Direction available: " << static_cast<int>(dir) << std::endl;
    }

    return 0;
}

// QuarterLang Sequencer — C++ Implementation
// Core: Sequentially executes a series of actions (functions, lambdas, or code blocks).
// Extendable: Supports insertion, removal, jumping, and state introspection.

#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include <stdexcept>

class Sequencer {
public:
    using Action = std::function<void(void)>;

    Sequencer() : current(0) {}

    // Add an action to the sequence
    void add(Action action, const std::string& label = "") {
        actions.push_back(action);
        labels.push_back(label);
    }

    // Insert an action at a specific position
    void insert(size_t index, Action action, const std::string& label = "") {
        if (index > actions.size()) throw std::out_of_range("Insert index out of range.");
        actions.insert(actions.begin() + index, action);
        labels.insert(labels.begin() + index, label);
    }

    // Remove an action by index
    void remove(size_t index) {
        if (index >= actions.size()) throw std::out_of_range("Remove index out of range.");
        actions.erase(actions.begin() + index);
        labels.erase(labels.begin() + index);
    }

    // Find index by label (returns -1 if not found)
    int findLabel(const std::string& label) const {
        for (size_t i = 0; i < labels.size(); ++i)
            if (!label.empty() && labels[i] == label) return static_cast<int>(i);
        return -1;
    }

    // Jump to a specific label
    void jumpTo(const std::string& label) {
        int idx = findLabel(label);
        if (idx == -1) throw std::runtime_error("Label not found: " + label);
        current = idx;
    }

    // Reset to start
    void reset() { current = 0; }

    // Run all remaining actions in order
    void run() {
        for (; current < actions.size(); ++current) {
            actions[current]();
        }
    }

    // Step one action
    void step() {
        if (current < actions.size()) {
            actions[current++]();
        }
    }

    // Introspect: get current position/label
    size_t position() const { return current; }
    std::string currentLabel() const { return current < labels.size() ? labels[current] : ""; }
    bool done() const { return current >= actions.size(); }
    size_t size() const { return actions.size(); }

private:
    std::vector<Action> actions;
    std::vector<std::string> labels;
    size_t current;
};

// === Demo Usage ===
int main() {
    Sequencer seq;

    seq.add([](){ std::cout << "star: Sequence started\n"; }, "start");
    seq.add([](){ std::cout << "Step 1: Preparing\n"; }, "prepare");
    seq.add([](){ std::cout << "Step 2: Executing\n"; }, "execute");
    seq.add([](){ std::cout << "Step 3: Finishing\n"; }, "finish");
    seq.add([](){ std::cout << "end: Sequence complete\n"; }, "end");

    // Run full sequence
    seq.run();

    // Reset and demo label jump
    seq.reset();
    std::cout << "\n-- Jumping to label 'execute' --\n";
    seq.jumpTo("execute");
    seq.run();

    return 0;
}

