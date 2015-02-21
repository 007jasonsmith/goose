# Required software:
# sudo apt-get install bochs-sdl

# TODO(chris): Automatically generate C_FILES, S_FILES, etc.
OBJECTS = kmain.o kmain_asm.o \
          klib/base.o klib/console.o klib/debug.o \
	  klib/strings.o \
	  sys/io.o \
	  sys/kernel_asm.o \
	  sys/gdt.o sys/gdt_asm.o \
	  sys/idt.o sys/idt_asm.o \
	  sys/isr.o sys/isr_asm.o

CC = gcc
CCFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
         -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c \
         -std=c11 \
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

run: os.iso
	bochs -f bochsrc.txt -q

%.o: %.c
	$(CC) $(CCFLAGS) $< -o $@

%o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf *.o        \
               ./klib/*.o \
               ./sys/*.o  \
               kernel.elf \
               os.iso     \
               iso/boot/kernel.elf
