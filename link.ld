ENTRY(loader)                /* the name of the entry label */

SECTIONS {
    /**
     * Memory offset for where to base the code from. We start at the 1MiB
     * mark so that we can reference hardware registers below that. We also
     * start at 0xC0000000 so that kernel memory stays in high-mem. The
     * AT(ADDR(...)) schenanigans is needed so that the data stays in
     * low memory. e.g. physical memory location vs. virtual memory location.
     */
    . = 0xC0100000;

    /**
     * Special multi-boot header section. This comes first so that the
     * magic values GRUB is looking for are present. Otherwise they may
     * not be in the first X KiB of the ELF binary, and GRUB may not
     * detect it is a multi-boot file.
     */
    .mbHeader ALIGN (0x1000) : AT(ADDR(.mbHeader) - 0xC0000000)
    {
         *(.mbHeader)
    }

    /**
     * All .text sections from all files.
     */
    .text ALIGN (0x1000) : AT(ADDR(.text) - 0xC0000000)
    {
        *(.text)
    }

    /**
     * All read-only sections from all files.
     */
    .rodata ALIGN (0x1000) : AT(ADDR(.rodata) - 0xC0000000)
    {
        *(.rodata*)
    }

    /**
     * All data sections from all files.
     */
    .data ALIGN (0x1000) : AT(ADDR(.data) - 0xC0000000)
    {
        *(.data)
    }

    /**
     * All COMMON and zero-initialized data sections from all files.
     */
    .bss ALIGN (0x1000) : AT(ADDR(.bss) - 0xC0000000)
    {
        *(COMMON)
        *(.bss)
    }
}
