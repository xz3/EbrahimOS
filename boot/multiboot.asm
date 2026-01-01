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

section .text
global _start
extern kernel_main

_start:
    cli
    call kernel_main
.hang:
    hlt
    jmp .hang
