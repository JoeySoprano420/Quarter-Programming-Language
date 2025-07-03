; IR Generator: Converts AST to DG-encoded IR
section .text
global generate_ir

generate_ir:
    ; Input: AST pointer in rdi
    ; Output: DG IR in memory
    ; Example: print "hi" → db 0xB8, 0x01, 0x00, 0x00, 0x00 ; mov eax, 1
    ; DG: [DG:SYSCALL_WRITE]
    ret
