#include <stdint.h>

struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct gdt_entry gdt[3];
struct gdt_ptr gdt_p;

void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[num].base_low    = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high   = (base >> 24) & 0xFF;
    gdt[num].limit_low   = (limit & 0xFFFF);
    gdt[num].granularity = (limit >> 16) & 0x0F;
    gdt[num].granularity |= gran & 0xF0;
    gdt[num].access      = access;
}

void gdt_init() {
    gdt_p.limit = (sizeof(struct gdt_entry) * 3) - 1;
    gdt_p.base  = (uint32_t)&gdt;

    // Gate 0: Null segment
    gdt_set_gate(0, 0, 0, 0, 0);                
    // Gate 1: Kernel Code segment (Selector 0x08)
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); 
    // Gate 2: Kernel Data segment (Selector 0x10)
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); 

    asm volatile("lgdt %0" : : "m"(gdt_p));

    // Reload segment registers to use the new GDT
    asm volatile(
        "ljmp $0x08, $.1\n"
        ".1:\n"
        "mov $0x10, %ax\n"
        "mov %ax, %ds\n"
        "mov %ax, %es\n"
        "mov %ax, %fs\n"
        "mov %ax, %gs\n"
        "mov %ax, %ss\n"
    );
}