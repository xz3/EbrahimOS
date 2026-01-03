ARCH = i386
CC   = gcc
LD   = ld
AS   = nasm

CFLAGS  = -m32 -ffreestanding -fno-pie -fno-stack-protector -O2 -Wall -Wextra \
          -Ikernel -Ikernel/interrupts -I.
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib

KERNEL   = kernel.bin
ISO      = Ebrahim-OS.iso
ISO_DIR  = iso
BOOT_DIR = $(ISO_DIR)/boot
GRUB_DIR = $(BOOT_DIR)/grub

OBJS = boot/multiboot.o \
       kernel/kernel.o kernel/vga.o kernel/gdt.o \
       kernel/interrupts/idt.o kernel/interrupts/irq.o kernel/interrupts/isr.o

all: $(KERNEL)

$(KERNEL): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(KERNEL)

boot/multiboot.o: boot/multiboot.asm
	$(AS) -f elf32 $< -o $@

kernel/interrupts/isr.o: kernel/interrupts/isr.asm
	$(AS) -f elf32 $< -o $@

kernel/kernel.o: kernel/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel/vga.o: kernel/vga.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel/gdt.o: kernel/interrupts/gdt.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel/interrupts/idt.o: kernel/interrupts/idt.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel/interrupts/irq.o: kernel/interrupts/irq.c
	$(CC) $(CFLAGS) -c $< -o $@

iso: $(KERNEL)
	mkdir -p $(GRUB_DIR)
	cp $(KERNEL) $(BOOT_DIR)
	cp boot/grub/grub.cfg $(GRUB_DIR)
	grub-mkrescue -o $(ISO) $(ISO_DIR)

run: iso
	qemu-system-i386 -no-reboot -no-shutdown -cdrom $(ISO)

clean:
	rm -rf boot/*.o kernel/*.o kernel/interrupts/*.o *.o $(KERNEL) $(ISO) $(ISO_DIR)
