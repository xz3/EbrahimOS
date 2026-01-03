BITS 32

global irq1
global irq7
global irq15

extern keyboard_handler
extern spurious_handler_master
extern spurious_handler_slave

irq1:
    pusha          ; Save all registers
    cld            ; Clear direction flag (C expects this)
    call keyboard_handler
    popa           ; Restore all registers
    iretd          ; Interrupt return

irq7:
    pusha
    cld
    call spurious_handler_master
    popa
    iretd

irq15:
    pusha
    cld
    call spurious_handler_slave
    popa
    iretd