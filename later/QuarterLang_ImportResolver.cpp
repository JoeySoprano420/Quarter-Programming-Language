#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <filesystem>

class ImportResolver {
public:
    std::unordered_map<std::string, std::string> fileContents;

    void load(const std::vector<std::string>& paths) {
        for (const auto& path : paths) {
            std::string id = std::filesystem::path(path).stem().string();
            fileContents[id] = QuarterProjectLoader::readFile(path);
        }
    }

    std::string resolve(const std::string& ns, const std::string& symbol) {
        std::string src = fileContents[ns];
        if (src.find(symbol) != std::string::npos)
            return "[✔️ found] " + symbol + " in " + ns;
        return "[❌ missing] " + symbol + " in " + ns;
    }
};
