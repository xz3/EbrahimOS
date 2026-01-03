#include <stdint.h>
#include "interrupts/idt.h"

// Forward declarations
void gdt_init(); 
void vga_clear();
void vga_print(const char* str);
void vga_set_color(uint8_t color);

void kernel_main(void) {
    gdt_init();   // <--- FIRST: Setup memory segments
    
    vga_set_color(0x1F);
    vga_clear();

    vga_print("========================================\n");
    vga_print("        E B R A H I M   O S\n");
    vga_print("========================================\n\n");

    idt_init();   // <--- SECOND: Setup interrupts
    
    vga_print("System Ready. Type something:\n> ");

    for (;;);
}