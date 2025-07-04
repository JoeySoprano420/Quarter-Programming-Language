Lexer lexer(code);
Parser parser(lexer.tokenize());
auto parsed = parser.parse();

AST ast;
for (const auto& stmt : parsed) {
    ast.addChild(stmt);
}

ast.print();
