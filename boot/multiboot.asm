BITS 32

section .multiboot
align 8

header_start:
    dd 0xe85250d6              ; multiboot2 magic
    dd 0                       ; architecture (i386)
    dd header_end - header_start
    dd -(0xe85250d6 + 0 + (header_end - header_start))

    ; end tag
    dw 0
    dw 0
    dd 8
header_end:

section .bss
align 16
stack_bottom:
    resb 16384        ; 16 KB stack
stack_top:

section .text
global _start
extern kernel_main

_start:
    cli
    mov esp, stack_top   ; 🔥 THIS IS WHAT YOU WERE MISSING
    mov ebp, esp

    call kernel_main

.hang:
    hlt
    jmp .hang
