# For information about required software and libraries,
# see tools.md.

OBJECTS = kmain.o kmain_asm.o \
          klib/argaccumulator.o klib/strings.o klib/type_printer.o \
          klib/debug.o klib/panic.o klib/print.o \
          sys/io.o sys/halt.o \
          sys/gdt.o sys/gdt_asm.o \
          sys/idt.o sys/idt_asm.o \
          sys/isr.o sys/isr_asm.o \
          sys/control_registers.o \
          kernel/memory.o kernel/memory2.o \
          kernel/boot.o kernel/elf.o \
          shell/shell.o \
          hal/keyboard.o hal/serial_port.o hal/text_ui.o

CPP = clang++

# For clang conversion
CPPFLAGS = -m32 \
           -nostdlib -nostdinc \
           -fno-builtin -fno-stack-protector \
           -Wreturn-type \
           -mno-mmx -mno-sse \
           -Wall -Wextra -Werror -c \
           -fno-exceptions -fno-rtti \
           -std=c++11 \
	   -I .

LD = ld
LDFLAGS = -T link.ld -melf_i386

AS = nasm
ASFLAGS = -f elf

all: kernel.elf

kernel.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o kernel.elf

os.iso: kernel.elf
	cp kernel.elf iso/boot/kernel.elf
	genisoimage -R                           \
		    -b boot/grub/stage2_eltorito \
                    -no-emul-boot                \
                    -boot-load-size 4            \
                    -A os                        \
                    -input-charset utf8          \
                    -quiet                       \
                    -boot-info-table             \
                    -o os.iso                    \
                    iso

run-bochs: os.iso
	bochs -f bochsrc.txt -q

run-qemu: os.iso
	qemu -boot d -cdrom os.iso -m 32 -serial file:qemu-com1.txt

%.o: %.cpp
	$(CPP) $(CPPFLAGS) $< -o $@

%o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf *.o          \
               ./hal/*.o    \
               ./sys/*.o    \
               ./klib/*.o   \
               ./shell/*.o  \
               ./kernel/*.o \
               ./bin/*      \
               kernel.elf   \
               os.iso       \
               iso/boot/kernel.elf
