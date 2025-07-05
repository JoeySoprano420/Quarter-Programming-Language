// QuarterLang_REPL.cpp
#include "QuarterLang_Runtime.cpp"
#include <string>

class QuarterREPL {
private:
    QuarterRuntime runtime;
    std::shared_ptr<ASTNode> root = std::make_shared<ASTNode>(ASTNodeType::ROOT);

public:
    void start() {
        std::cout << "🌀 QuarterLang REPL — type `exit` to quit\n";
        std::string line;

        while (true) {
            std::cout << "⏳ > ";
            std::getline(std::cin, line);
            if (line == "exit") break;

            Lexer lexer(line);
            Parser parser(lexer.tokenize());
            auto nodes = parser.parse();

            for (auto& n : nodes) root->children.push_back(n);

            runtime.execute(root);
        }
    }
};
