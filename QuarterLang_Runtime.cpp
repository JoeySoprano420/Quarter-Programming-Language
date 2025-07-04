// QuarterLang_Runtime.cpp
#pragma once
#include "QuarterLang_AST.cpp"
#include <unordered_map>
#include <iostream>

class QuarterRuntime {
private:
    std::unordered_map<std::string, std::string> memory;
    std::unordered_map<std::string, bool> constants;

public:
    void execute(const std::shared_ptr<ASTNode>& root) {
        for (const auto& child : root->children) {
            executeNode(child);
        }
    }

private:
    void executeNode(const std::shared_ptr<ASTNode>& node) {
        switch (node->type) {
            case ASTNodeType::VAL_DECL:
                handleValDecl(node);
                break;
            case ASTNodeType::VAR_DECL:
                handleVarDecl(node);
                break;
            case ASTNodeType::TRUTHS_DECL:
                handleTruths(node);
                break;
            case ASTNodeType::PROOFS_DECL:
                handleProofs(node);
                break;
            case ASTNodeType::LOOP_STMT:
                handleLoop(node);
                break;
            default:
                break;
        }
    }

    void handleValDecl(const std::shared_ptr<ASTNode>& node) {
        std::string varName = node->value;
        std::string type = node->children[0]->value;
        std::string val = node->children[1]->value;
        memory[varName] = val;
        constants[varName] = true;
        std::cout << "[val] " << varName << " = " << val << " (" << type << ")\n";
    }

    void handleVarDecl(const std::shared_ptr<ASTNode>& node) {
        std::string varName = node->value;
        std::string type = node->children[0]->value;
        std::string val = node->children[1]->value;
        memory[varName] = val;
        constants[varName] = false;
        std::cout << "[var] " << varName << " = " << val << " (" << type << ")\n";
    }

    void handleTruths(const std::shared_ptr<ASTNode>& node) {
        std::cout << "[truths] ";
        for (const auto& t : node->children)
            std::cout << t->value << " ";
        std::cout << "\n";
    }

    void handleProofs(const std::shared_ptr<ASTNode>& node) {
        std::string proofType = node->value;
        std::string lhs = node->children[0]->value;
        std::string rhs = node->children[1]->value;

        std::cout << "[proofs] " << proofType << " ";
        std::cout << lhs << " vs " << rhs << "\n";
    }

    void handleLoop(const std::shared_ptr<ASTNode>& node) {
        int start = std::stoi(node->children[0]->value);
        int end = std::stoi(node->children[1]->value);
        std::cout << "[loop] from " << start << " to " << end << "\n";
        for (int i = start; i <= end; ++i)
            std::cout << "  ➜ Iteration: " << i << "\n";
    }
};
