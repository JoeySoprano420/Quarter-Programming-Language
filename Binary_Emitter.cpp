// BinaryEmitter.h
#pragma once
#include <cstdlib>
#include <string>

class BinaryEmitter {
public:
    void assemble(const std::string& asmFile, const std::string& exeFile) {
        std::string nasmCmd = "nasm -f win64 " + asmFile + " -o output.obj";
        std::string linkCmd = "link /subsystem:console /entry:main output.obj /OUT:" + exeFile;
        system(nasmCmd.c_str());
        system(linkCmd.c_str());
    }
};

