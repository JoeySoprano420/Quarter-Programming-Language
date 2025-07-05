// QuarterLang_Lexer.cpp
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>

enum TokenType {
    T_KEYWORD, T_IDENTIFIER, T_NUMBER, T_STRING,
    T_OPERATOR, T_COLON, T_NEWLINE, T_EOF
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
};

class Lexer {
private:
    std::string source;
    size_t current = 0;
    int line = 1;
    std::vector<Token> tokens;

    std::unordered_map<std::string, TokenType> keywords = {
        {"val", T_KEYWORD}, {"var", T_KEYWORD}, {"loop", T_KEYWORD},
        {"truths", T_KEYWORD}, {"proofs", T_KEYWORD},
        {"dg", T_KEYWORD}, {"dgvec", T_KEYWORD},
        {"bool", T_KEYWORD}, {"types", T_KEYWORD}
    };

public:
    Lexer(const std::string& src) : source(src) {}

    std::vector<Token> tokenize() {
        while (!isAtEnd()) {
            char c = advance();

            if (std::isspace(c)) {
                if (c == '\n') line++;
                continue;
            }

            if (std::isalpha(c)) {
                std::string word = readWhile(isAlphaNumeric);
                TokenType type = keywords.count(word) ? keywords[word] : T_IDENTIFIER;
                tokens.push_back({type, word, line});
            } else if (std::isdigit(c)) {
                std::string num = readWhile(isDigit);
                tokens.push_back({T_NUMBER, num, line});
            } else if (c == '"') {
                std::string str = readString();
                tokens.push_back({T_STRING, str, line});
            } else if (c == ':') {
                tokens.push_back({T_COLON, ":", line});
            } else {
                tokens.push_back({T_OPERATOR, std::string(1, c), line});
            }
        }

        tokens.push_back({T_EOF, "", line});
        return tokens;
    }

private:
    bool isAtEnd() const { return current >= source.length(); }

    char advance() { return source[current++]; }

    char peek() const { return source[current]; }

    static bool isAlphaNumeric(char c) {
        return std::isalnum(c) || c == '_';
    }

    static bool isDigit(char c) {
        return std::isdigit(c);
    }

    std::string readWhile(bool (*condition)(char)) {
        std::string result;
        while (!isAtEnd() && condition(source[current])) {
            result += advance();
        }
        return result;
    }

    std::string readString() {
        std::string str;
        while (!isAtEnd() && peek() != '"') {
            str += advance();
        }
        advance(); // Consume closing quote
        return str;
    }
};
