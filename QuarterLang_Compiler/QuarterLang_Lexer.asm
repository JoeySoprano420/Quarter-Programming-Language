; Lexer: Tokenizes Quarter source into lexemes
section .text
global lex_line

lex_line:
    ; Input: rsi = pointer to line
    ; Output: tokens in memory (DG-encoded)
    ; Example: "print 'hello'" → DG[PRINT][STR_HELLO]
    ; Inline hex: db 0x01, 0x10, 0x48, 0x65, 0x6C, 0x6C, 0x6F
    ; DG: [DG:PRINT][DG:STR_HELLO]
    ret
