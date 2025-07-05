// QuarterLang_Optimizer.cpp
#pragma once
#include "QuarterLang_IRBytecode.cpp"
#include <map>

class Optimizer {
private:
    std::vector<IRInstruction> optimized;

    // Optional: Constant environment
    std::map<std::string, std::string> constValues;

public:
    std::vector<IRInstruction> optimize(const std::vector<IRInstruction>& input) {
        optimized.clear();
        for (const auto& instr : input) {
            switch (instr.op) {
                case IROpcode::IR_VAL:
                    if (constValues.count(instr.arg)) {
                        // Eliminate redundant constant
                        log("Removed duplicate const: " + instr.arg);
                        continue;
                    }
                    constValues[instr.arg] = findNextConst(input, instr.arg);
                    optimized.push_back(instr);
                    break;

                case IROpcode::IR_VAR:
                    optimized.push_back(instr); // keep mutable vars
                    break;

                case IROpcode::IR_LOAD_INT:
                    if (instr.arg == "0" || instr.arg == "1") {
                        // Micro-optimize common constants
                        IRInstruction fast = instr;
                        fast.hex = intToHex(std::stoi(instr.arg));
                        optimized.push_back(fast);
                    } else {
                        optimized.push_back(instr);
                    }
                    break;

                case IROpcode::IR_LOOP:
                    optimized.push_back(instr); // May inline loop unrolling later
                    break;

                case IROpcode::IR_TRUTH:
                case IROpcode::IR_PROOF:
                case IROpcode::IR_DG_SYMBOL:
                    // All symbolic, logic-side ops: keep as-is
                    optimized.push_back(instr);
                    break;

                case IROpcode::IR_LOAD_STR:
                    if (!isPrintWorthy(instr.arg)) {
                        log("Removed unused string: " + instr.arg);
                        continue;
                    }
                    optimized.push_back(instr);
                    break;

                default:
                    optimized.push_back(instr);
                    break;
            }
        }

        return optimized;
    }

private:
    std::string findNextConst(const std::vector<IRInstruction>& instrs, const std::string& var) {
        for (size_t i = 0; i < instrs.size(); ++i) {
            if (instrs[i].op == IROpcode::IR_VAL && instrs[i].arg == var) {
                if (i + 2 < instrs.size()) {
                    if (instrs[i + 2].op == IROpcode::IR_LOAD_INT)
                        return instrs[i + 2].arg;
                }
            }
        }
        return "";
    }

    std::string intToHex(int val) {
        std::stringstream ss;
        ss << std::hex << val;
        return ss.str();
    }

    bool isPrintWorthy(const std::string& s) {
        return !(s.find("unused") != std::string::npos || s.empty());
    }

    void log(const std::string& msg) {
        std::cout << "[OPT] " << msg << std::endl;
    }
};
