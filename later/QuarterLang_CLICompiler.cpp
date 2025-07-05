// QuarterLang_CLICompiler.cpp
#include "QuarterLang_Lexer.cpp"
#include "QuarterLang_Parser.cpp"
#include "QuarterLang_AST.cpp"
#include "QuarterLang_IRBytecode.cpp"
#include "QuarterLang_Optimizer.cpp"
#include "QuarterLang_CodeGenerator.cpp"
#include "QuarterLang_BinaryEmitter.cpp"

#include <fstream>

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "❌ Cannot open source file: " << path << std::endl;
        exit(1);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: qtrc <source.qtr>\n";
        return 1;
    }

    std::string sourcePath = argv[1];
    std::string asmOutput = "output.asm";

    std::cout << "🔍 Reading source: " << sourcePath << "\n";
    std::string source = readFile(sourcePath);

    // 1️⃣ LEXER
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    std::cout << "✅ Lexing complete: " << tokens.size() << " tokens\n";

    // 2️⃣ PARSER
    Parser parser(tokens);
    auto astNodes = parser.parse();
    std::cout << "✅ Parsing complete: " << astNodes.size() << " root-level nodes\n";

    // 3️⃣ AST WRAP
    AST ast;
    for (auto& node : astNodes)
        ast.addChild(node);

    // Optional: Debug tree
    ast.print();

    // 4️⃣ IR GEN
    IRGenerator irgen;
    auto rawIR = irgen.generate(ast.root);
    std::cout << "✅ IR generated: " << rawIR.size() << " instructions\n";

    // 5️⃣ OPTIMIZER
    Optimizer opt;
    auto optimizedIR = opt.optimize(rawIR);
    std::cout << "✅ Optimization done: " << optimizedIR.size() << " instructions remain\n";

    // 6️⃣ NASM EMITTER
    CodeGenerator gen(optimizedIR);
    gen.generate(asmOutput);

    // 7️⃣ BINARY EMITTER
    BinaryEmitter be(asmOutput);
    be.build();

    return 0;
}

#include "QuarterLang_ProjectLoader.cpp"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: qtrc <file.qtr | project.qtrproj>\n";
        return 1;
    }

    std::string entry = argv[1];
    std::vector<std::string> files;

    if (entry.ends_with(".qtrproj")) {
        files = QuarterProjectLoader::loadSourcesFromProject(entry);
    } else {
        files.push_back(entry);
    }

    AST ast;
    for (const auto& f : files) {
        std::string code = QuarterProjectLoader::readFile(f);
        Lexer lexer(code);
        Parser parser(lexer.tokenize());
        auto nodes = parser.parse();
        for (auto& n : nodes)
            ast.addChild(n);
    }

    std::cout << "📦 Project parsed: " << files.size() << " files\n";

    IRGenerator irgen;
    auto rawIR = irgen.generate(ast.root);

    Optimizer opt;
    auto optIR = opt.optimize(rawIR);

    CodeGenerator codegen(optIR);
    codegen.generate("output.asm");

    BinaryEmitter emitter("output.asm");
    emitter.build();

    return 0;
}

