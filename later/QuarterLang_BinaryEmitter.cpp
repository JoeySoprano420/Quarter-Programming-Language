// QuarterLang_BinaryEmitter.cpp
#pragma once
#include <cstdlib>
#include <iostream>
#include <string>
#include <filesystem>

#ifdef _WIN32
#define PLATFORM_WINDOWS
#else
#define PLATFORM_UNIX
#endif

class BinaryEmitter {
private:
    std::string asmFile;
    std::string objFile;
    std::string exeFile;

public:
    BinaryEmitter(const std::string& asmPath) : asmFile(asmPath) {
        objFile = replaceExt(asmFile, ".obj");
        exeFile = replaceExt(asmFile, ".exe");
    }

    void build() {
        std::cout << "📦 Building from: " << asmFile << std::endl;

        if (!std::filesystem::exists(asmFile)) {
            std::cerr << "❌ Assembly file not found: " << asmFile << std::endl;
            return;
        }

        if (!assemble()) return;
        if (!link()) return;

        std::cout << "✅ Binary ready: " << exeFile << std::endl;
    }

private:
    bool assemble() {
#ifdef PLATFORM_WINDOWS
        std::string cmd = "nasm -f win64 " + asmFile + " -o " + objFile;
#else
        std::string cmd = "nasm -f elf64 " + asmFile + " -o " + objFile;
#endif
        std::cout << "⚙️ Assembling: " << cmd << std::endl;
        return system(cmd.c_str()) == 0;
    }

    bool link() {
#ifdef PLATFORM_WINDOWS
        std::string cmd = "link.exe /ENTRY:main /SUBSYSTEM:CONSOLE /OUT:" + exeFile + " " + objFile + " /NODEFAULTLIB /DEFAULTLIB:libcmt.lib /DEFAULTLIB:kernel32.lib /DEFAULTLIB:user32.lib";
#else
        std::string cmd = "ld -o " + exeFile + " " + objFile + " -e main -lc";
#endif
        std::cout << "🔗 Linking: " << cmd << std::endl;
        return system(cmd.c_str()) == 0;
    }

    std::string replaceExt(const std::string& path, const std::string& newExt) {
        auto dot = path.find_last_of('.');
        if (dot == std::string::npos) return path + newExt;
        return path.substr(0, dot) + newExt;
    }
};
