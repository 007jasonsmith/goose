MAGIC_NUMBER equ 0x1BADB002     ; Magic number for bootloader.
FLAGS        equ 0x0            ; Multiboot flags
CHECKSUM     equ -MAGIC_NUMBER  ; Calculate the checksum
                                ; (magic number + checksum + flags should equal 0)

KERNEL_STACK_SIZE equ 4096      ; Stack size in bytes

; This is the virtual base address of kernel space. It will be used to convert
; virtual addresses into physical addresses until paging is enabled. Note that
; this is not the virtual address where the kernel image itself is loaded --
; just the amount that must be subtracted from a virtual address to get a
; physical address. (See link.ld for the actual address.)
KERNEL_VIRTUAL_BASE equ 0xC0000000                  ; 3GiB
KERNEL_PAGE_NUMBER equ (KERNEL_VIRTUAL_BASE >> 22)  ; Page directory index of
				                    ; kernel's 4MB PTE.

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

; This is called from link.ld, to begin execution from GRUB. Since we link the
; kernel binary to be in high memory, we need to actually place the code in
; low memory until we enable paging. 
global loader
loader equ (actual_loader - KERNEL_VIRTUAL_BASE)

actual_loader:
    ; NOTE: Paging isn't enabled, so all addresses must be physical and/or
    ; relative addresses. Also, do NOT threash eax or ebx, since GRUB puts
    ; boot data there.

    ; Load Page Directory base register.
    mov ecx, (boot_page_directory_table - KERNEL_VIRTUAL_BASE)
    mov cr3, ecx

    ; Set PSE bit in CR4 to enable 4MiB pages.
    mov ecx, cr4
    or ecx, 0x00000010
    mov cr4, ecx

    ; Set PG bit in CR0 to enable paging.
    mov ecx, cr0
    or ecx, 0x80000000
    mov cr0, ecx
 
    ; Start fetching instructions in kernel space. Huzzah!
    ; eip holds the physical address of this command ~0x00100000, so we need
    ; to do a long jump to the correct virtual address in high memory.
    lea ecx, [start_in_higher_half]
    jmp ecx

start_in_higher_half:
    ; Now that eip is correctly fetching instructions ~0xC0100000, we no
    ; longer need to keep the identity pages for 0x00000000.
    mov dword [boot_page_directory_table], 0
    mov dword [boot_page_directory_table+4], 0
    invlpg [0]
    invlpg [1]

    mov esp, kernel_stack+KERNEL_STACK_SIZE           ; set up the stack

    ; From here on, paging must bee enabled. Note that ONLY the first 4MiB of
    ; physical address space is mapped to KERNEL_VIRTUAL_BASE. So if the kernel
    ; grows beyond that, you're going to have a bad time. (Unless you can set up
    ; paging in the Kernel.)

    push ebx			; pass multiboot-info struct
    push eax                    ; pass Multiboot magic number
 
    call kmain
    call system_halt


section .bss
align 4                         ; align at 4 bytes
kernel_stack:                   ; label points to the beginning of memory
        resb KERNEL_STACK_SIZE  ; reserve stack for the kernel

    ; TODO(chris): Reclaim. Takes up 4KiB of memory but isn't used post-boot.
section .data
align 0x1000                    ; align page directory table at 4KiB
boot_page_directory_table:	
    ; We initialize a page directory table with only two regions. One identity
    ; mapping the first few MiB (where our kernel exists in physical memory),
    ; and another page in high memory pointing to the first few of physical memory
    ; (where the kernel will be virtualy referenced). We need the lower-memory
    ; identity page, otherwise things will crash as soon as paging is enabled.
    ; Because the next instruction pointer will be invalid.
    ;
    ; All bits are clear except the following:
    ; bit 7: PS The kernel page is 4MB.
    ; bit 1: RW The kernel page is read/write.
    ; bit 0: P  The kernel page is present.
    ;
    ; We map two pages in each region, since our kernel takes up more than
    ; 4 MiB. Be sure to update start_in_higher_half when changing this.
    dd 0x00000083				; low-memory page
    dd 0x00400083				; low-memory page
    dd 0x00800083				; low-memory page
    times (KERNEL_PAGE_NUMBER - 3) dd 0       	; pages before kernel space
    dd 0x00000083				; high-memory kernel page
    dd 0x00400083				; high-memory kernel page
    dd 0x00800083				; high-memory kernel page
    times (1024 - KERNEL_PAGE_NUMBER - 3) dd 0  ; pages after the kernel image.
