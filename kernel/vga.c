#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_ADDRESS 0xB8000

static uint16_t* const VGA_BUFFER = (uint16_t*) VGA_ADDRESS;

static uint8_t cursor_row = 0;
static uint8_t cursor_col = 0;
static uint8_t vga_color = 0x0F; // white on black

static inline uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t) c | (uint16_t) color << 8;
}

void vga_clear() {
    for (uint16_t y = 0; y < VGA_HEIGHT; y++) {
        for (uint16_t x = 0; x < VGA_WIDTH; x++) {
            VGA_BUFFER[y * VGA_WIDTH + x] = vga_entry(' ', vga_color);
        }
    }
    cursor_row = 0;
    cursor_col = 0;
}

void vga_putc(char c) {
    if (c == '\n') {
        cursor_col = 0;
        cursor_row++;
        return;
    }

    VGA_BUFFER[cursor_row * VGA_WIDTH + cursor_col] = vga_entry(c, vga_color);
    cursor_col++;

    if (cursor_col >= VGA_WIDTH) {
        cursor_col = 0;
        cursor_row++;
    }
}

void vga_print(const char* str) {
    for (int i = 0; str[i]; i++) {
        vga_putc(str[i]);
    }
}

void vga_set_color(uint8_t color) {
    vga_color = color;
}
