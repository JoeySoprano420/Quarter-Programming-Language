// QuarterLang_ProjectLoader.cpp
#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>

class QuarterProjectLoader {
public:
    static std::vector<std::string> loadSourcesFromProject(const std::string& projPath) {
        std::ifstream file(projPath);
        if (!file) {
            std::cerr << "❌ Failed to open .qtrproj file: " << projPath << "\n";
            exit(1);
        }

        std::vector<std::string> sources;
        std::string line;
        std::string dir = std::filesystem::path(projPath).parent_path().string();

        while (std::getline(file, line)) {
            if (!line.empty()) {
                std::string fullPath = dir + "/" + line;
                if (!std::filesystem::exists(fullPath)) {
                    std::cerr << "❌ Missing file: " << fullPath << "\n";
                    exit(1);
                }
                sources.push_back(fullPath);
            }
        }

        return sources;
    }

    static std::string readFile(const std::string& path) {
        std::ifstream file(path);
        if (!file) {
            std::cerr << "❌ Cannot open: " << path << "\n";
            exit(1);
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
};
