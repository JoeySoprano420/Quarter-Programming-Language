struct QtrFile {
    std::string name;
    std::string path;
    bool isOpen = false;
    bool isMain = false;
};

class QuarterIDEProjectManager {
private:
    std::vector<QtrFile> files;

public:
    void loadProject(const std::string& qtrprojPath) {
        auto paths = QuarterProjectLoader::loadSourcesFromProject(qtrprojPath);
        for (const auto& p : paths) {
            QtrFile f;
            f.path = p;
            f.name = std::filesystem::path(p).filename().string();
            f.isMain = f.name == "main.qtr";
            files.push_back(f);
        }
    }

    void display() {
        std::cout << "📂 Project Tree\n";
        for (const auto& f : files)
            std::cout << (f.isMain ? "▶️ " : "   ") << f.name << "\n";
    }

    std::vector<std::string> getPaths() const {
        std::vector<std::string> out;
        for (const auto& f : files) out.push_back(f.path);
        return out;
    }
};
