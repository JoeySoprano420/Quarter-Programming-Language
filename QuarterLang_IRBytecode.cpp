// QuarterLang_IRBytecode.cpp
#pragma once
#include "QuarterLang_AST.cpp"
#include <sstream>
#include <iomanip>
#include <unordered_map>

enum class IROpcode {
    IR_VAL, IR_VAR, IR_LOOP, IR_TRUTH, IR_PROOF,
    IR_LOAD_STR, IR_LOAD_INT, IR_DG_SYMBOL, IR_NOP
};

struct IRInstruction {
    IROpcode op;
    std::string arg;
    std::string hex; // inline hex if applicable (for NASM)

    std::string str() const {
        std::stringstream ss;
        ss << "[" << opcodeToStr(op) << "] ";
        if (!arg.empty()) ss << arg << " ";
        if (!hex.empty()) ss << "| 0x" << hex;
        return ss.str();
    }

    static std::string opcodeToStr(IROpcode op) {
        switch (op) {
            case IROpcode::IR_VAL: return "VAL";
            case IROpcode::IR_VAR: return "VAR";
            case IROpcode::IR_LOOP: return "LOOP";
            case IROpcode::IR_TRUTH: return "TRUTH";
            case IROpcode::IR_PROOF: return "PROOF";
            case IROpcode::IR_LOAD_STR: return "LOAD_STR";
            case IROpcode::IR_LOAD_INT: return "LOAD_INT";
            case IROpcode::IR_DG_SYMBOL: return "DODECAGRAM";
            default: return "NOP";
        }
    }
};

class IRGenerator {
private:
    std::vector<IRInstruction> instructions;

public:
    std::vector<IRInstruction> generate(const std::shared_ptr<ASTNode>& node) {
        walk(node);
        return instructions;
    }

    void walk(const std::shared_ptr<ASTNode>& node) {
        switch (node->type) {
            case ASTNodeType::VAL_DECL:
                emit(IROpcode::IR_VAL, node->value);
                walk(node->children[0]); // type
                walk(node->children[1]); // value
                break;
            case ASTNodeType::VAR_DECL:
                emit(IROpcode::IR_VAR, node->value);
                walk(node->children[0]);
                walk(node->children[1]);
                break;
            case ASTNodeType::INT_LITERAL:
                emit(IROpcode::IR_LOAD_INT, node->value, intToHex(std::stoi(node->value)));
                break;
            case ASTNodeType::STRING_LITERAL:
                emit(IROpcode::IR_LOAD_STR, node->value, strToHex(node->value));
                break;
            case ASTNodeType::IDENTIFIER:
                // Potential dodecagram encoding if symbol is DG
                if (isDodecaGram(node->value)) {
                    emit(IROpcode::IR_DG_SYMBOL, node->value, encodeDodecaGram(node->value));
                } else {
                    emit(IROpcode::IR_LOAD_STR, node->value);
                }
                break;
            case ASTNodeType::TRUTHS_DECL:
                for (const auto& child : node->children)
                    emit(IROpcode::IR_TRUTH, child->value);
                break;
            case ASTNodeType::PROOFS_DECL:
                emit(IROpcode::IR_PROOF, node->value);
                walk(node->children[0]);
                walk(node->children[1]);
                break;
            case ASTNodeType::LOOP_STMT:
                emit(IROpcode::IR_LOOP, "loop_start");
                walk(node->children[0]); // from
                walk(node->children[1]); // to
                break;
            case ASTNodeType::ROOT:
                for (const auto& child : node->children)
                    walk(child);
                break;
            default:
                emit(IROpcode::IR_NOP, "unknown_node");
        }
    }

private:
    void emit(IROpcode op, const std::string& arg = "", const std::string& hex = "") {
        instructions.push_back({op, arg, hex});
    }

    std::string intToHex(int val) {
        std::stringstream ss;
        ss << std::hex << val;
        return ss.str();
    }

    std::string strToHex(const std::string& str) {
        std::stringstream ss;
        for (char c : str)
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)c;
        return ss.str();
    }

    // Very simple Dodecagram check (e.g., A1B, 9A2)
    bool isDodecaGram(const std::string& val) {
        return val.length() == 3 && std::isalnum(val[0]) && std::isalnum(val[1]) && std::isalnum(val[2]);
    }

    std::string encodeDodecaGram(const std::string& dg) {
        // Simplified encoding: base-12 symbol → hex
        std::stringstream ss;
        for (char c : dg)
            ss << std::hex << int(c);
        return ss.str(); // mock; real DG encoding could follow your DodecaGrams.md spec
    }
};
