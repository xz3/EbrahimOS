#include <stdint.h>

void vga_clear();
void vga_print(const char* str);
void vga_set_color(uint8_t color);

void kernel_main(void) {
    vga_set_color(0x1F); // white on blue
    vga_clear();

    vga_set_color(0x1F);
    vga_print("========================================\n");
    vga_print("        E B R A H I M   O S\n");
    vga_print("              DESKTOP v0.1\n");
    vga_print("========================================\n\n");

    vga_set_color(0x0F);
    vga_print("[ Terminal ]   [ Files ]   [ Settings ]\n\n");

    vga_set_color(0x0A);
    vga_print("> ");

    for (;;);
}
