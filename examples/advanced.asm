; Advanced assembly example demonstrating sections, data, memory ops, and jumps

.data
    counter     dq 10           ; 64-bit counter
    message     db 72, 101, 108, 108, 111  ; "Hello" in ASCII
    buffer      resb 64         ; Reserve 64 bytes
    
.text
_start:
    mov rax, counter            ; Load address of counter (placeholder)
    mov rbx, 5                  ; Load immediate value
    add rax, rbx                ; Add values
    cmp rax, 15                 ; Compare with 15
    je equal_label              ; Jump if equal
    jg greater_label            ; Jump if greater
    
loop_start:
    sub rbx, 1                  ; Decrement counter
    cmp rbx, 0                  ; Check if zero
    jne loop_start              ; Jump if not zero
    
equal_label:
    mov rcx, 42                 ; Set success value
    jmp end_program             ; Jump to end
    
greater_label:
    mov rcx, 99                 ; Set different value
    
end_program:
    nop                         ; No operation
    ret                         ; Return 