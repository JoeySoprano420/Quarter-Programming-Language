// QuarterLang_AST.cpp
#pragma once
#include <string>
#include <vector>
#include <memory>
#include <iostream>

enum class ASTNodeType {
    VAL_DECL, VAR_DECL, LOOP_STMT,
    TRUTHS_DECL, PROOFS_DECL,
    FUNC_DECL, STRING_LITERAL,
    INT_LITERAL, IDENTIFIER,
    ROOT
};

std::string ASTNodeTypeToString(ASTNodeType type) {
    switch (type) {
        case ASTNodeType::VAL_DECL: return "ValDecl";
        case ASTNodeType::VAR_DECL: return "VarDecl";
        case ASTNodeType::LOOP_STMT: return "LoopStmt";
        case ASTNodeType::TRUTHS_DECL: return "TruthsDecl";
        case ASTNodeType::PROOFS_DECL: return "ProofsDecl";
        case ASTNodeType::FUNC_DECL: return "FuncDecl";
        case ASTNodeType::STRING_LITERAL: return "StringLiteral";
        case ASTNodeType::INT_LITERAL: return "IntLiteral";
        case ASTNodeType::IDENTIFIER: return "Identifier";
        case ASTNodeType::ROOT: return "Root";
        default: return "Unknown";
    }
}

struct ASTNode {
    ASTNodeType type;
    std::string value;
    std::vector<std::shared_ptr<ASTNode>> children;

    ASTNode(ASTNodeType t, const std::string& v = "") : type(t), value(v) {}
};

class AST {
public:
    std::shared_ptr<ASTNode> root;

    AST() {
        root = std::make_shared<ASTNode>(ASTNodeType::ROOT);
    }

    void addChild(const std::shared_ptr<ASTNode>& node) {
        root->children.push_back(node);
    }

    void print() const {
        std::cout << "AST Tree:" << std::endl;
        printNode(root, 0);
    }

private:
    void printNode(const std::shared_ptr<ASTNode>& node, int indent) const {
        for (int i = 0; i < indent; ++i) std::cout << "  ";
        std::cout << ASTNodeTypeToString(node->type);
        if (!node->value.empty())
            std::cout << ": " << node->value;
        std::cout << std::endl;

        for (const auto& child : node->children) {
            printNode(child, indent + 1);
        }
    }
};
