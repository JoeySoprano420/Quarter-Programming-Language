// QuarterLang_Parser.cpp
#include "QuarterLang_Lexer.cpp"
#include <memory>
#include <iostream>

enum class ASTNodeType {
    VAL_DECL, VAR_DECL, LOOP_STMT, TRUTHS_DECL, PROOFS_DECL,
    FUNC_DECL, STRING_LITERAL, INT_LITERAL, IDENTIFIER
};

struct ASTNode {
    ASTNodeType type;
    std::string value;
    std::vector<std::shared_ptr<ASTNode>> children;

    ASTNode(ASTNodeType t, const std::string& v = "") : type(t), value(v) {}
};

class Parser {
private:
    std::vector<Token> tokens;
    size_t current = 0;

public:
    explicit Parser(const std::vector<Token>& toks) : tokens(toks) {}

    std::vector<std::shared_ptr<ASTNode>> parse() {
        std::vector<std::shared_ptr<ASTNode>> statements;
        while (!isAtEnd()) {
            auto stmt = parseStatement();
            if (stmt) statements.push_back(stmt);
        }
        return statements;
    }

private:
    bool isAtEnd() const { return peek().type == T_EOF; }

    Token peek() const { return tokens[current]; }

    Token advance() { return tokens[current++]; }

    bool match(TokenType type) {
        if (peek().type == type) {
            advance();
            return true;
        }
        return false;
    }

    Token expect(TokenType type, const std::string& msg) {
        if (match(type)) return tokens[current - 1];
        std::cerr << "Parse error on line " << peek().line << ": " << msg << std::endl;
        exit(1);
    }

    std::shared_ptr<ASTNode> parseStatement() {
        if (match(T_KEYWORD)) {
            std::string keyword = tokens[current - 1].lexeme;

            if (keyword == "val" || keyword == "var") {
                return parseVarDecl(keyword == "val");
            }
            if (keyword == "loop") {
                return parseLoop();
            }
            if (keyword == "truths") {
                return parseTruths();
            }
            if (keyword == "proofs") {
                return parseProofs();
            }
        }

        // Unknown or unsupported statement
        advance();
        return nullptr;
    }

    std::shared_ptr<ASTNode> parseVarDecl(bool isConst) {
        auto name = expect(T_IDENTIFIER, "Expected variable name");
        expect(T_KEYWORD, "Expected 'as'");
        auto type = expect(T_IDENTIFIER, "Expected type name");
        expect(T_COLON, "Expected ':'");
        auto value = advance(); // Could be number or string

        auto decl = std::make_shared<ASTNode>(isConst ? ASTNodeType::VAL_DECL : ASTNodeType::VAR_DECL, name.lexeme);
        decl->children.push_back(std::make_shared<ASTNode>(ASTNodeType::IDENTIFIER, type.lexeme));
        decl->children.push_back(std::make_shared<ASTNode>(
            value.type == T_STRING ? ASTNodeType::STRING_LITERAL : ASTNodeType::INT_LITERAL, value.lexeme));
        return decl;
    }

    std::shared_ptr<ASTNode> parseLoop() {
        expect(T_IDENTIFIER, "Expected 'from'");
        auto start = advance(); // number
        expect(T_IDENTIFIER, "Expected 'to'");
        auto end = advance();   // number
        expect(T_COLON, "Expected ':'");

        auto loop = std::make_shared<ASTNode>(ASTNodeType::LOOP_STMT);
        loop->children.push_back(std::make_shared<ASTNode>(ASTNodeType::INT_LITERAL, start.lexeme));
        loop->children.push_back(std::make_shared<ASTNode>(ASTNodeType::INT_LITERAL, end.lexeme));
        return loop;
    }

    std::shared_ptr<ASTNode> parseTruths() {
        expect(T_COLON, "Expected ':' after 'truths'");
        std::vector<std::shared_ptr<ASTNode>> truthNodes;
        while (!isAtEnd() && peek().type == T_IDENTIFIER) {
            auto id = advance();
            truthNodes.push_back(std::make_shared<ASTNode>(ASTNodeType::IDENTIFIER, id.lexeme));
        }
        auto truths = std::make_shared<ASTNode>(ASTNodeType::TRUTHS_DECL);
        truths->children = truthNodes;
        return truths;
    }

    std::shared_ptr<ASTNode> parseProofs() {
        auto validate = advance(); // usually "validate"
        auto lhs = advance();      // e.g. gravity
        auto against = advance();  // e.g. "against"
        auto rhs = advance();      // e.g. mass

        auto proofs = std::make_shared<ASTNode>(ASTNodeType::PROOFS_DECL, validate.lexeme);
        proofs->children.push_back(std::make_shared<ASTNode>(ASTNodeType::IDENTIFIER, lhs.lexeme));
        proofs->children.push_back(std::make_shared<ASTNode>(ASTNodeType::IDENTIFIER, rhs.lexeme));
        return proofs;
    }
};
