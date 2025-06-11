; Test file demonstrating working assembler features

.text
start:
    mov rax, 100                ; 64-bit immediate to register
    mov rbx, rax                ; register to register transfer
    mov ecx, 42                 ; 32-bit immediate to register
    
    ; Arithmetic operations
    add rax, 10                 ; add immediate to register
    sub rbx, 5                  ; subtract immediate from register
    cmp rax, rbx                ; compare two registers
    
    ; Control flow
    je equal                    ; jump if equal
    jne not_equal              ; jump if not equal
    jl less_than               ; jump if less than
    jg greater_than            ; jump if greater than
    
equal:
    mov rdx, 1                  ; set result to 1
    jmp end                     ; unconditional jump
    
not_equal:
    mov rdx, 2                  ; set result to 2
    jmp end                     ; unconditional jump
    
less_than:
    mov rdx, 3                  ; set result to 3
    jmp end                     ; unconditional jump
    
greater_than:
    mov rdx, 4                  ; set result to 4
    
end:
    nop                         ; no operation
    ret                         ; return 