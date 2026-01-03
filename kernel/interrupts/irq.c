#include <stdint.h>
#include "irq.h"
#include "idt.h"

/* Provided by your VGA driver */
extern void vga_putc(char c);

/* ===== PS/2 scancode -> ASCII (set 1, basic) ===== */
static const char scancode_table[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,   'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,  '\\','z','x','c','v','b','n','m',',','.','/',
    0,   '*', 0,  ' '
};

/* ===== PORT IO ===== */
static inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void io_wait(void) {
    asm volatile("outb %%al, $0x80" : : "a"(0));
}

/* ===== PIC constants ===== */
#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

#define PIC_EOI      0x20

/* Read PIC In-Service Register (ISR) for spurious IRQ detection */
static uint16_t pic_get_isr(void) {
    outb(PIC1_COMMAND, 0x0B);
    outb(PIC2_COMMAND, 0x0B);
    return ((uint16_t)inb(PIC2_COMMAND) << 8) | inb(PIC1_COMMAND);
}

/* Flush any pending bytes from the PS/2 controller output buffer */
static void keyboard_init(void) {
    while (inb(0x64) & 0x01) {
        (void)inb(0x60);
    }
}

/* ===== PIC remap =====
   Master IRQs -> vectors 0x20..0x27
   Slave  IRQs -> vectors 0x28..0x2F
*/
void pic_remap(void) {
    /* Start initialization sequence (ICW1) */
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);
    io_wait();

    /* ICW2: vector offsets */
    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);
    io_wait();

    /* ICW3: tell Master there is a slave at IRQ2 (0000 0100),
       tell Slave its cascade identity (0000 0010) */
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);
    io_wait();

    /* ICW4: 8086 mode */
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);
    io_wait();

    /* Masks:
       0xF9 = 11111001b -> unmask IRQ1 (keyboard) + IRQ2 (cascade), mask everything else
       Slave fully masked for now (0xFF)
    */
    outb(PIC1_DATA, 0xF9);
    outb(PIC2_DATA, 0xFF);

    keyboard_init();
}

/* ===== IRQ handlers called from isr.asm ===== */
void keyboard_handler(void) {
    uint8_t scancode = inb(0x60);

    /* Ignore key releases (bit 7 set) */
    if (scancode & 0x80) {
        outb(PIC1_COMMAND, PIC_EOI);
        return;
    }

    char c = 0;
    if (scancode < 128) c = scancode_table[scancode];
    if (c) vga_putc(c);

    /* EOI to master PIC */
    outb(PIC1_COMMAND, PIC_EOI);
}

/* You may not be using this yet, but irq.h declares it */
void timer_handler(void) {
    /* TODO: tick++ etc */
    outb(PIC1_COMMAND, PIC_EOI);
}

/* Proper spurious handling:
   - IRQ7 spurious: if ISR bit 7 not set, it was spurious => no EOI
   - IRQ15 spurious: if ISR bit 15 not set, spurious => EOI to master only
*/
void spurious_handler_master(void) {
    uint16_t isr = pic_get_isr();
    if (!(isr & (1u << 7))) {
        return; /* spurious IRQ7 */
    }
    outb(PIC1_COMMAND, PIC_EOI); /* real IRQ7 */
}

void spurious_handler_slave(void) {
    uint16_t isr = pic_get_isr();
    if (!(isr & (1u << 15))) {
        outb(PIC1_COMMAND, PIC_EOI); /* spurious IRQ15: EOI master only */
        return;
    }
    /* real IRQ15: EOI slave then master */
    outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
}
