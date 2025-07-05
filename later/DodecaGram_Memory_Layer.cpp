struct DodecaGram {
    std::string symbol; // base-12 symbolic e.g., A9B
    int value;          // simplified numeric mapping
};

class DodecaMemory {
private:
    std::unordered_map<std::string, DodecaGram> dgrams;
    std::unordered_map<std::string, std::vector<DodecaGram>> dvecs;

public:
    void storeDG(const std::string& id, const std::string& sym) {
        dgrams[id] = {sym, mapDG(sym)};
    }

    void storeDGVec(const std::string& id, const std::vector<std::string>& symbols) {
        std::vector<DodecaGram> vec;
        for (const auto& s : symbols) vec.push_back({s, mapDG(s)});
        dvecs[id] = vec;
    }

    void dump() const {
        std::cout << "[🔷 DG Memory]\n";
        for (const auto& [id, dg] : dgrams)
            std::cout << "  dg " << id << " = " << dg.symbol << " (" << dg.value << ")\n";
        for (const auto& [id, vec] : dvecs) {
            std::cout << "  dgvec " << id << " = [ ";
            for (const auto& g : vec)
                std::cout << g.symbol << " ";
            std::cout << "]\n";
        }
    }

private:
    int mapDG(const std::string& sym) {
        int total = 0;
        for (char c : sym) total += static_cast<int>(c);
        return total % 144; // simplistic base-12 mapped value
    }
};
