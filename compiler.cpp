#pragma once

// A scoped enum for all possible token kinds.
// 'enum class' avoids polluting the global namespace
// and prevents accidental comparisons with integers.
enum class TokenType {
	EndOfFile,
	Identifier,
	Number,
	Plus,
	Minus,
	Multiply,
	Divide,
	LeftParen,
	RightParen,
	// … add more kinds as needed
};

// A simple struct to represent a token.
struct Token {
	TokenType type; // The type of the token
	std::string value; // The string value of the token, if applicable
	int line; // The line number where the token was found
	int column; // The column number where the token was found
	// Constructor to initialize a token
	Token(TokenType t, const std::string& v, int l, int c)
		: type(t), value(v), line(l), column(c) {}
};
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <cctype>
#include <sstream>
// A simple lexer class to tokenize
class Lexer {
	public:
	Lexer(const std::string& source)
		: source(source), position(0), line(1), column(1) {}
	Token nextToken() {
		skipWhitespace();
		if (position >= source.size()) {
			return Token(TokenType::EndOfFile, "", line, column);
		}
		char current = source[position];
		if (std::isalpha(current)) {
			return identifier();
		} else if (std::isdigit(current)) {
			return number();
		} else if (current == '+') {
			return consumeToken(TokenType::Plus);
		} else if (current == '-') {
			return consumeToken(TokenType::Minus);
		} else if (current == '*') {
			return consumeToken(TokenType::Multiply);
		} else if (current == '/') {
			return consumeToken(TokenType::Divide);
		} else if (current == '(') {
			return consumeToken(TokenType::LeftParen);
		} else if (current == ')') {
			return consumeToken(TokenType::RightParen);
		} else {
			throw std::runtime_error("Unexpected character: " + std::string(1, current));
		}
	}
	void skipWhitespace() {
		while (position < source.size() && std::isspace(source[position])) {
			if (source[position] == '\n') {
				line++;
				column = 1;
			} else {
				column++;
			}
			position++;
		}
	}
	Token consumeToken(TokenType type) {
		char current = source[position];
		std::string value(1, current);
		position++;
		column++;
		return Token(type, value, line, column - 1);
	}
	Token identifier() {
		int start = position;
		while (position < source.size() && std::isalnum(source[position])) {
			position++;
			column++;
		}
		std::string value = source.substr(start, position - start);
		return Token(TokenType::Identifier, value, line, column - value.size());
	}
	Token number() {
		int start = position;
		while (position < source.size() && std::isdigit(source[position])) {
			position++;
			column++;
		}
		std::string value = source.substr(start, position - start);
		return Token(TokenType::Number, value, line, column - value.size());
	}
private:
	std::string source; // The source code to tokenize
	size_t position; // Current position in the source code
	int line; // Current line number
	int column; // Current column number
};
// A simple parser class to parse tokens
class Parser {
	public:
	Parser(Lexer& lexer) : lexer(lexer), currentToken(lexer.nextToken()) {}
	void parse() {
		while (currentToken.type != TokenType::EndOfFile) {
			expression();
		}
	}
	void expression() {
		// For simplicity, we just print the tokens in an expression
		if (currentToken.type == TokenType::Identifier || currentToken.type == TokenType::Number) {
			std::cout << "Token: " << currentToken.value << " at line " << currentToken.line << ", column " << currentToken.column << std::endl;
			currentToken = lexer.nextToken();
		} else {
			throw std::runtime_error("Expected identifier or number, found: " + currentToken.value);
		}
	}
	void nextToken() {
		currentToken = lexer.nextToken();
	}
private:
	Lexer& lexer; // Reference to the lexer
	Token currentToken; // The current token being parsed
};
// A simple main function to demonstrate the lexer and parser
int main() {
	std::string source = "x = 42 + 3 * (y - 5)";
	Lexer lexer(source);
	Parser parser(lexer);
	try {
		parser.parse();
	} catch (const std::runtime_error& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}
// This code provides a basic structure for a lexer and parser in C++.
// It can be extended with more features like error handling, more token types,
// and a more complex grammar for parsing expressions.

#include <iostream>
#include <string>
#include <stdexcept>

// Token types
enum class TokenType {
	Identifier,
	Number,
	EndOfFile
};

// Token structure
struct Token {
	TokenType type;
	std::string value;
	int line;
	int column;

	Token(TokenType t, const std::string& v, int l, int c)
		: type(t), value(v), line(l), column(c) {
	}
};

// Lexer class
class Lexer {
public:
	Lexer(const std::string& src) : source(src), position(0), line(1), column(1) {}

	Token nextToken() {
		while (position < source.size() && isspace(source[position])) {
			if (source[position] == '\n') {
				line++;
				column = 1;
			}
			else {
				column++;
			}
			position++;
		}

		if (position >= source.size()) {
			return Token(TokenType::EndOfFile, "", line, column);
		}

		size_t start = position;
		if (isalpha(source[position])) {
			while (position < source.size() && isalnum(source[position])) {
				position++;
				column++;
			}
			std::string value = source.substr(start, position - start);
			return Token(TokenType::Identifier, value, line, column - value.size());
		}

		if (isdigit(source[position])) {
			while (position < source.size() && isdigit(source[position])) {
				position++;
				column++;
			}
			std::string value = source.substr(start, position - start);
			return Token(TokenType::Number, value, line, column - value.size());
		}

		throw std::runtime_error("Unknown character: " + std::string(1, source[position]));
	}

private:
	std::string source;
	size_t position;
	int line;
	int column;
};

// Parser class
class Parser {
public:
	Parser(Lexer& lex) : lexer(lex), currentToken(lexer.nextToken()) {}

	void parse() {
		while (currentToken.type != TokenType::EndOfFile) {
			expression();
		}
	}

private:
	void expression() {
		if (currentToken.type == TokenType::Identifier || currentToken.type == TokenType::Number) {
			std::cout << "Token: " << currentToken.value
				<< " at line " << currentToken.line
				<< ", column " << currentToken.column << std::endl;
			currentToken = lexer.nextToken();
		}
		else {
			throw std::runtime_error("Expected identifier or number, found: " + currentToken.value);
		}
	}

	Lexer& lexer;
	Token currentToken;
};

// Main function
int main() {
	std::string source = "x = 42 + 3 * (y - 5)";
	Lexer lexer(source);
	Parser parser(lexer);

	try {
		parser.parse();
	}
	catch (const std::runtime_error& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}

