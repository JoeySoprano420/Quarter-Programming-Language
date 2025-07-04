Lexer lexer(code);
Parser parser(lexer.tokenize());
auto nodes = parser.parse();

AST ast;
for (auto& n : nodes)
    ast.addChild(n);

QuarterRuntime rt;
rt.execute(ast.root);
