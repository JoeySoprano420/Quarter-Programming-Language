// qtrcapsule_run.cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include <cstdlib>
#include <zlib.h> // Ensure zlib is available
#ifdef _WIN32
#include <windows.h>
#endif

#define CAPSULE_HEADER "QTRC2.1"
#define ASM_FILE "extracted_compiler.asm"
#define OUTPUT_FILE "compiler"

std::string decompress(const std::string& data) {
    std::stringstream out;
    z_stream zs{};
    inflateInit(&zs);
    zs.avail_in = data.size();
    zs.next_in = (Bytef*)data.data();
    char buffer[32768];
    do {
        zs.avail_out = sizeof(buffer);
        zs.next_out = reinterpret_cast<Bytef*>(buffer);
        inflate(&zs, Z_NO_FLUSH);
        out.write(buffer, sizeof(buffer) - zs.avail_out);
    } while (zs.avail_out == 0);
    inflateEnd(&zs);
    return out.str();
}

bool runCommand(const std::string& cmd) {
    std::cout << "🔧 Running: " << cmd << std::endl;
    return system(cmd.c_str()) == 0;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: qtrcapsule_run <capsule.qtrcapsule>\n";
        return 1;
    }

    std::ifstream in(argv[1], std::ios::binary);
    if (!in) {
        std::cerr << "❌ Cannot open capsule: " << argv[1] << std::endl;
        return 1;
    }

    std::string header(7, '\0');
    in.read(&header[0], 7);
    if (header != CAPSULE_HEADER) {
        std::cerr << "❌ Invalid capsule header.\n";
        return 1;
    }

    std::stringstream compressed;
    compressed << in.rdbuf();
    std::string decompressed = decompress(compressed.str());

    // Split capsule sections
    size_t src_start = decompressed.find(":::QTR_SOURCE:::");
    size_t asm_start = decompressed.find(":::QTR_ASM:::");
    size_t meta_start = decompressed.find(":::QTR_META:::");
    if (src_start == std::string::npos || asm_start == std::string::npos) {
        std::cerr << "❌ Capsule format corrupted.\n";
        return 1;
    }

    std::string src = decompressed.substr(src_start + 16, asm_start - (src_start + 16));
    std::string asmcode = decompressed.substr(asm_start + 13, meta_start - (asm_start + 13));

    std::ofstream asmfile(ASM_FILE);
    asmfile << asmcode;
    asmfile.close();
    std::cout << "📦 Extracted: " << ASM_FILE << std::endl;

#ifdef _WIN32
    std::string obj = "compiler.obj";
    std::string exe = OUTPUT_FILE + std::string(".exe");
    if (!runCommand("nasm -f win64 " + std::string(ASM_FILE) + " -o " + obj)) return 1;
    if (!runCommand("link /ENTRY:main /OUT:" + exe + " " + obj + " /SUBSYSTEM:CONSOLE /NODEFAULTLIB /DEFAULTLIB:libcmt.lib /DEFAULTLIB:kernel32.lib")) return 1;
    runCommand(exe);
#else
    std::string obj = "compiler.o";
    std::string out = OUTPUT_FILE + std::string(".out");
    if (!runCommand("nasm -f elf64 " + std::string(ASM_FILE) + " -o " + obj)) return 1;
    if (!runCommand("ld " + obj + " -o " + out)) return 1;
    runCommand("./" + out);
#endif

    return 0;
}
