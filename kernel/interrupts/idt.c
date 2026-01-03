#include "idt.h"
#include <stdint.h>

extern void irq1();
extern void irq7();
extern void irq15();
extern void pic_remap();

static struct idt_entry idt[256];
static struct idt_ptr idt_reg;

static void idt_set_gate(int n, uint32_t handler) {
    idt[n].base_low  = handler & 0xFFFF;
    idt[n].base_high = (handler >> 16) & 0xFFFF;
    idt[n].selector  = 0x08; // Kernel code segment
    idt[n].zero      = 0;
    idt[n].flags     = 0x8E; // Present, Ring 0, 32-bit Interrupt Gate
}

void idt_init() {
    asm volatile("cli"); // Disable interrupts during setup

    // Clear all entries to prevent garbage execution
    for (int i = 0; i < 256; i++) {
        idt[i].base_low  = 0;
        idt[i].base_high = 0;
        idt[i].selector  = 0;
        idt[i].zero      = 0;
        idt[i].flags     = 0;
    }

    // Set valid gates
    idt_set_gate(33, (uint32_t)irq1);   // Keyboard IRQ
    idt_set_gate(39, (uint32_t)irq7);   // Spurious Master
    idt_set_gate(47, (uint32_t)irq15);  // Spurious Slave

    idt_reg.limit = sizeof(idt) - 1;
    idt_reg.base  = (uint32_t)&idt;
    
    pic_remap();
    
    asm volatile("lidt %0" : : "m"(idt_reg)); // Load IDT register
    asm volatile("sti"); // Re-enable interrupts
}