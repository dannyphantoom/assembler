; Memory operations test

.text
main:
    ; Basic memory operations
    mov rax, 100                ; Load immediate
    mov rbx, rax                ; Copy register
    
    ; Memory operations (when implemented)
    ; mov [rsp], rax            ; Store to memory
    ; mov rdx, [rsp]            ; Load from memory
    ; mov [rsp + 8], rbx        ; Store with offset
    
    ; Arithmetic operations
    add rax, 10                 ; Add immediate
    sub rbx, 5                  ; Subtract immediate
    cmp rax, rbx                ; Compare registers
    
    ; Conditional operations
    je done                     ; Jump if equal
    jne done                    ; Jump if not equal
    jl done                     ; Jump if less
    jg done                     ; Jump if greater
    
done:
    ret                         ; Return 