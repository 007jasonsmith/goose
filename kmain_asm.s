MAGIC_NUMBER equ 0x1BADB002     ; Magic number for bootloader.
FLAGS        equ 0x0            ; Multiboot flags
CHECKSUM     equ -MAGIC_NUMBER  ; Calculate the checksum
                                ; (magic number + checksum + flags should equal 0)

KERNEL_STACK_SIZE equ 4096      ; Stack size in bytes

extern kmain                    ; Defined in kmain.c
extern system_halt		; Defined in sys/halt.h
	
section .mbHeader		; IMPORTANT: The multi-boot header needs
                           	; to be in the first 8K, otherwise Grub won't
	          		; notice it.
align 4
	dd MAGIC_NUMBER         ; write the magic number to the machine code,
        dd FLAGS                ; the flags,
        dd CHECKSUM             ; and the checksum
	
section .text                   ; start of the text (code) section
align 4                         ; the code must be 4 byte aligned

global loader
loader:                         ; the loader label (entry point from linker.ldt)
	; TODO(chris): Stop using this and rely on a different boot loader.
	push ebx                ; Where GRUB stores the Memory Map info.
	push eax                ; Where GRUB stores the multiboot info magic number.
	call kmain
	call system_halt	; If the kernel does return. Shut it down.

section .bss
align 4                         ; align at 4 bytes
kernel_stack:                   ; label points to the beginning of memory
        resb KERNEL_STACK_SIZE  ; reserve stack for the kernel