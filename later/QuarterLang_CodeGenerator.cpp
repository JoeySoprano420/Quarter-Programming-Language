// QuarterLang_CodeGenerator.cpp
#pragma once
#include "QuarterLang_IRBytecode.cpp"
#include <fstream>

class CodeGenerator {
private:
    std::vector<IRInstruction> instructions;
    std::stringstream nasm;

public:
    explicit CodeGenerator(const std::vector<IRInstruction>& ir) : instructions(ir) {}

    void generate(const std::string& outputPath = "output.asm") {
        emitHeader();
        emitData();
        emitText();
        writeFile(outputPath);
    }

private:
    void emitHeader() {
        nasm << "bits 64\n";
        nasm << "global main\n";
        nasm << "extern printf\n";
        nasm << "\nsection .data\n";
    }

    void emitData() {
        for (const auto& instr : instructions) {
            if (instr.op == IROpcode::IR_LOAD_STR) {
                std::string label = "msg_" + sanitize(instr.arg);
                nasm << label << " db \"" << instr.arg << "\", 0x0A, 0\n";
            }
        }
    }

    void emitText() {
        nasm << "\nsection .text\n";
        nasm << "main:\n";
        for (const auto& instr : instructions) {
            switch (instr.op) {
                case IROpcode::IR_VAL:
                case IROpcode::IR_VAR:
                    nasm << "  ; " << instr.opcodeToStr(instr.op) << " " << instr.arg << "\n";
                    break;

                case IROpcode::IR_LOAD_INT:
                    nasm << "  mov rax, " << instr.arg << "   ; INT | 0x" << instr.hex << "\n";
                    break;

                case IROpcode::IR_LOAD_STR: {
                    std::string label = "msg_" + sanitize(instr.arg);
                    nasm << "  lea rdi, [" << label << "]\n";
                    nasm << "  xor rax, rax\n";
                    nasm << "  call printf\n";
                    break;
                }

                case IROpcode::IR_TRUTH:
                    nasm << "  ; [TRUTH] " << instr.arg << "\n";
                    break;

                case IROpcode::IR_PROOF:
                    nasm << "  ; [PROOF] " << instr.arg << "\n";
                    break;

                case IROpcode::IR_LOOP:
                    emitLoop(instr);
                    break;

                case IROpcode::IR_DG_SYMBOL:
                    nasm << "  ; DodecaGram: " << instr.arg << " | 0x" << instr.hex << "\n";
                    break;

                default:
                    nasm << "  ; [NOP]\n";
                    break;
            }
        }

        // Exit
        nasm << "  mov rax, 60\n";
        nasm << "  xor rdi, rdi\n";
        nasm << "  syscall\n";
    }

    void emitLoop(const IRInstruction& instr) {
        static int loopId = 0;
        std::string id = std::to_string(loopId++);
        nasm << "  mov rcx, 5     ; loop hardcoded\n";
        nasm << "loop_" << id << ":\n";
        nasm << "  ; Loop body for loop_" << id << "\n";
        nasm << "  loop loop_" << id << "\n";
    }

    std::string sanitize(const std::string& s) {
        std::string out;
        for (char c : s) {
            if (isalnum(c)) out += c;
            else out += '_';
        }
        return out;
    }

    void writeFile(const std::string& path) {
        std::ofstream file(path);
        if (!file) {
            std::cerr << "❌ Failed to open " << path << " for writing.\n";
            return;
        }
        file << nasm.str();
        file.close();
        std::cout << "✅ NASM written to: " << path << "\n";
    }
};
