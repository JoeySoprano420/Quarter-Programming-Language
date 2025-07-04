// QuarterLang_ExecutionPane.cpp
#pragma once
#include "QuarterLang_Runtime.cpp"
#include <thread>
#include <chrono>
#include <atomic>

class ExecutionPane {
private:
    std::atomic<bool> running;
    std::string currentCode;
    std::thread execThread;

public:
    ExecutionPane() : running(false) {}

    void start(const std::string& code) {
        currentCode = code;
        if (running) stop();
        running = true;

        execThread = std::thread([&]() {
            while (running) {
                clearScreen();
                std::cout << "🔁 Live QuarterLang Execution\n";
                executeCode(currentCode);
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        });
    }

    void updateCode(const std::string& newCode) {
        currentCode = newCode;
    }

    void stop() {
        running = false;
        if (execThread.joinable()) execThread.join();
    }

    ~ExecutionPane() {
        stop();
    }

private:
    void executeCode(const std::string& code) {
        Lexer lexer(code);
        auto tokens = lexer.tokenize();

        Parser parser(tokens);
        auto nodes = parser.parse();

        AST ast;
        for (const auto& n : nodes) ast.addChild(n);

        QuarterRuntime rt;
        rt.execute(ast.root);
    }

    void clearScreen() {
#ifdef _WIN32
        system("cls");
#else
        std::cout << "\033[2J\033[1;1H"; // ANSI clear
#endif
    }
};
