ARCH=x86_64
CC=gcc
LD=ld
AS=nasm

CFLAGS = -m32 -ffreestanding -fno-pie -fno-stack-protector -O2 -Wall -Wextra
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib


ISO_DIR=iso
BOOT_DIR=$(ISO_DIR)/boot
GRUB_DIR=$(BOOT_DIR)/grub

KERNEL=kernel.bin

all: $(KERNEL)

multiboot.o:
	$(AS) -f elf32 boot/multiboot.asm -o multiboot.o


kernel.o:
	$(CC) $(CFLAGS) -c kernel/kernel.c -o kernel.o
	$(CC) $(CFLAGS) -c kernel/vga.c -o vga.o


$(KERNEL): multiboot.o kernel.o
	$(LD) $(LDFLAGS) multiboot.o kernel.o vga.o -o $(KERNEL)


iso: $(KERNEL)
	mkdir -p $(BOOT_DIR) $(GRUB_DIR)
	cp $(KERNEL) $(BOOT_DIR)
	cp boot/grub/grub.cfg $(GRUB_DIR)
	GRUB_PLATFORM=i386-pc grub-mkrescue -o Ebrahim-OS.iso iso



run: iso
	qemu-system-x86_64 -cdrom Ebrahim-OS.iso -boot d


clean:
	rm -rf *.o *.bin iso *.iso
