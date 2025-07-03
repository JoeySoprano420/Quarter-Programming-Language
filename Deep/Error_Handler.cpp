// ErrorHandler.h
#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>

struct Error {
    enum class Stage {
        Lexer, Parser, Binder, IRGen, CodeGen, Runtime
    };
    Stage stage;
    std::string message;
    int line;
    int col;
    std::string snippet; // relevant source code
};

class ErrorHandler {
    std::vector<Error> errors;
public:
    void add(Error::Stage stage, const std::string& msg, int line = -1, int col = -1, const std::string& snippet = "") {
        errors.push_back({stage, msg, line, col, snippet});
    }

    bool hasErrors() const { return !errors.empty(); }

    void report() const {
        if (errors.empty()) return;
        std::cerr << "\n\x1b[1;31mQuarterLang: Compilation Failed\x1b[0m\n";
        for (const auto& err : errors) {
            std::cerr << "  [";
            std::cerr << stageToStr(err.stage) << " Error";
            if (err.line >= 0) std::cerr << " @ line " << err.line;
            if (err.col >= 0) std::cerr << ", col " << err.col;
            std::cerr << "]\n    ";
            std::cerr << err.message << "\n";
            if (!err.snippet.empty()) {
                std::cerr << "    > " << highlight(err.snippet, err.col) << "\n";
            }
            std::cerr << "\n";
        }
    }

    void throwIfErrors() const {
        if (hasErrors()) {
            report();
            exit(1);
        }
    }

private:
    static std::string stageToStr(Error::Stage s) {
        switch (s) {
            case Error::Stage::Lexer: return "Lexer";
            case Error::Stage::Parser: return "Parser";
            case Error::Stage::Binder: return "Binder";
            case Error::Stage::IRGen:  return "IRGen";
            case Error::Stage::CodeGen:return "CodeGen";
            case Error::Stage::Runtime:return "Runtime";
        }
        return "Unknown";
    }
    // Optionally highlight the error column with a caret
    static std::string highlight(const std::string& line, int col) {
        if (col < 0 || (size_t)col > line.size()) return line;
        std::ostringstream out;
        out << line << "\n    ";
        for (int i = 0; i < col - 1; ++i) out << ' ';
        out << "\x1b[1;32m^\x1b[0m";
        return out.str();
    }
};
