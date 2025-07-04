// QuarterLang_IDEPlugin.cpp
#pragma once
#include "QuarterLang_CLICompiler.cpp"
#include <filesystem>
#include <thread>
#include <chrono>

class QuarterIDEPlugin {
private:
    std::string currentFile;
    std::string diagnostics;
    std::string compileOutput;
    bool autoBuild = true;

public:
    QuarterIDEPlugin() {}

    void openFile(const std::string& path) {
        currentFile = path;
        std::cout << "📂 Opened: " << path << std::endl;
        if (autoBuild) triggerBuild();
    }

    void toggleAutoBuild(bool enable) {
        autoBuild = enable;
        std::cout << "⚙️ AutoBuild: " << (autoBuild ? "ON" : "OFF") << std::endl;
    }

    void triggerBuild() {
        std::cout << "🛠️ Compiling: " << currentFile << std::endl;
        runCompiler(currentFile);
    }

    std::string getDiagnostics() const { return diagnostics; }
    std::string getAssemblyOutput() const { return compileOutput; }

private:
    void runCompiler(const std::string& sourceFile) {
        // 1. Read source
        std::string src = readFile(sourceFile);
        Lexer lexer(src);
        auto tokens = lexer.tokenize();

        Parser parser(tokens);
        auto astNodes = parser.parse();

        AST ast;
        for (auto& node : astNodes)
            ast.addChild(node);

        IRGenerator irgen;
        auto ir = irgen.generate(ast.root);

        Optimizer opt;
        auto optimized = opt.optimize(ir);

        CodeGenerator cg(optimized);
        std::string asmPath = "ide_output.asm";
        cg.generate(asmPath);
        compileOutput = readFile(asmPath);

        BinaryEmitter be(asmPath);
        be.build();

        // Populate diagnostics with dummy error messages
        diagnostics = "[Diagnostics]\n";
        for (auto& token : tokens) {
            if (token.lexeme == "ERROR") {
                diagnostics += "⚠️ Syntax error at line " + std::to_string(token.line) + "\n";
            }
        }
    }
};

#include "QuarterLang_ExecutionPane.cpp"
ExecutionPane pane;
pane.start(sourceCode);     // start live running
pane.updateCode(newCode);   // refresh with new edits
pane.stop();                // clean shutdown

#include "QuarterLang_ExecutionPane.cpp"

class QuarterIDEPlugin {
private:
    ExecutionPane execPane;
    std::string currentSource;

public:
    void loadFile(const std::string& path) {
        currentSource = readFile(path);
        execPane.start(currentSource);  // 💥 Live execution from IDE
    }

    void updateCode(const std::string& newCode) {
        currentSource = newCode;
        execPane.updateCode(newCode);  // 💥 Hot swap running code
    }

    void stopExecution() {
        execPane.stop();
    }

    void showDGState(const DodecaMemory& mem) {
        mem.dump(); // ⬅ Optional memory inspector
    }
};

