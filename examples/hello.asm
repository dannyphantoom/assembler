; Simple x86-64 assembly program demonstrating Intel syntax
; This program shows basic MOV instructions with different operand types

main:
    mov rax, 0x1234567890ABCDEF  ; 64-bit immediate to register
    mov rbx, rax                 ; register to register transfer
    mov ecx, 42                  ; 32-bit immediate to register
    mov edx, ecx                 ; 32-bit register to register
    
    ; Memory operations (basic examples)
    ; mov rax, [rbx]             ; TODO: implement memory operands
    ; mov [rbp + 8], rax         ; TODO: implement memory operands
    
    ; No operation
    nop
    
    ; Return instruction
    ret 