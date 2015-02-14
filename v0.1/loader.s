    global loader                   ; the entry symbol for ELF

    MAGIC_NUMBER equ 0x1BADB002     ; define the magic number constant
    FLAGS        equ 0x0            ; multiboot flags
    CHECKSUM     equ -MAGIC_NUMBER  ; calculate the checksum
                                    ; (magic number + checksum + flags should equal 0)

    KERNEL_STACK_SIZE equ 4096      ; stack size in bytes

    extern kmain                    ; defined in kmain.c

    section .mbHeader		    ; IMPORTANT: The multi-boot header needs
                           	    ; to be in the first 8K, otherwise Grub won't
	          		    ; notice it.
    align 4
	dd MAGIC_NUMBER             ; write the magic number to the machine code,
        dd FLAGS                    ; the flags,
        dd CHECKSUM                 ; and the checksum
	
    section .text                   ; start of the text (code) section
    align 4                         ; the code must be 4 byte aligned

    loader:                         ; the loader label (defined as entry point in linker script)
	call kmain
	
    .loop:                           ; loop forever.
        jmp .loop

    section .bss
    align 4                         ; align at 4 bytes
    kernel_stack:                   ; label points to the beginning of memory
        resb KERNEL_STACK_SIZE      ; reserve stack for the kernel
