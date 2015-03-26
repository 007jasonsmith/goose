# Memory #

Memory is kinda important. This document outlines how it works in Goose.

## Booting ##

The system's physical memory is tallied as part of the booting process.
GRUB uses BIOS methods to determine the total system memory, as well as build
a memory map of what is already in use.

Not all system memory is available, because some is already reserved for
memory-mapped hardware registers, the kernel ELF binary itself, and so on.

See:
- http://wiki.osdev.org/Memory_Map_(x86)
- http://wiki.osdev.org/Detecting_Memory_(x86)
- http://wiki.osdev.org/Detecting_Memory_(x86)#Getting_a_GRUB_Memory_Map

The GRUB memory map is passed as the first parameter to the kernel's main
method.

## Enabling ##

With the memory map available, it is possible to use "real mode" to read and
write to memory. However, that obviously isn't going to scale. So the next
step is to enable paging.

However, what do we do about the memory that is already located at a physical
address as indicated by the memory map?

See:
- However, with the kernel already loaded
