// NASMEmitter.h
#pragma once
#include "IR.h"
#include <string>
class NASMEmitter {
public:
    void emit(const IRFunc& func, const std::string& filename) {
        std::ofstream out(filename);
        out << "section .data\n";
        int msgNum = 1;
        for (const auto& instr : func.body)
            if (instr.op == IROp::Say)
                out << "msg" << msgNum++ << " db '" << instr.arg << "',0\n";
        out << "\nsection .text\nglobal main\nmain:\n";
        msgNum = 1;
        for (const auto& instr : func.body) {
            if (instr.op == IROp::Say) {
                out << "    mov rcx, msg" << msgNum << "   ; 48 8D 0D ...\n";
                out << "    call print_string            ; E8 ...\n";
                msgNum++;
            } else if (instr.op == IROp::Ret) {
                out << "    mov eax, 0                   ; B8 00 00 00 00\n";
                out << "    ret                          ; C3\n";
            }
        }
        out.close();
    }
};
