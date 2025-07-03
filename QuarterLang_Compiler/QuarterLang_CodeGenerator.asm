; Code Generator: Emits NASM with inline hex
section .text
global emit_nasm

emit_nasm:
    ; Input: DG IR
    ; Output: NASM + inline hex
    ; Example:
    ;   mov eax, 60         ; db 0xB8, 0x3C, 0x00, 0x00, 0x00
    ;   xor edi, edi        ; db 0x31, 0xFF
    ;   syscall             ; db 0x0F, 0x05
    ret
