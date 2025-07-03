// IR.h
#pragma once
#include <string>
#include <vector>

enum class IROp {
    Say, Ret, // ...add more as QuarterLang grows
};

struct IRInstr {
    IROp op;
    std::string arg;
};

struct IRFunc {
    std::string name;
    std::vector<IRInstr> body;
};

// IRGenerator.h
#pragma once
#include "AST.h"
#include "IR.h"

class IRGenerator {
public:
    IRFunc generate(FuncDecl* ast) {
        IRFunc func;
        func.name = ast->name;
        for (const auto& stmt : ast->body) {
            if (auto say = dynamic_cast<SayStmt*>(stmt.get()))
                func.body.push_back({IROp::Say, say->text});
            else if (dynamic_cast<RetStmt*>(stmt.get()))
                func.body.push_back({IROp::Ret, ""});
        }
        return func;
    }
};
