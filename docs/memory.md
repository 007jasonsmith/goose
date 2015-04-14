# Memory #

Memory is kinda important. This document outlines how it works in Goose.

## Booting ##

The system's physical memory is tallied as part of the booting process. The boot
loader (GRUB) uses BIOS methods to determine the total system memory, as well as
build a memory map of what is already in use.

Not all system memory is available, because some is already reserved for
memory-mapped hardware registers. Note that the memory map from the boot
loader does __not__ include information about where the ELF binary is loaded.

The memory map is passed as the first parameter to the kernel's main
method. (Since the contract is it is stored in a register just before
transfering control to the ELF binary.

### Paging v1  ###

Before control is transfered to the kernel's main method, paging is enabled
so that the kernel can be loaded into higher memory. (e.g. above 0xC0000000).

The kernel is linked in such a way that pointer addresses start at 0xC0100000,
which is 0xC0000000 + 1MiB.

So when paging is first enabled, it identity maps the first 8MiB of memory (so
the instruction pointer will be valid) and also maps
the first 8MiB of 0xC0000000 to the first 8MiB of physical memory. (So the
kernel code will work as expected.)

Once control has trasnfered to kernel code located in high-memory, the first
few page directory table entries (identity mapping the first few MiB of memroy)
are zeroed out.

## Setting up Heapspace ##

Setting up heap space requires some bootstrapping.

### Paging v2 ###

Once the kernel code (C++) has been loaded, we then set up paging again, but
this time using a more accurate representation of the data. As well as have the
ability to allocate new page frames, etc.

This is done by using a statically alocated (.bss segment) kernel page directory
table and the corresponding page tables for high memory (0xC0000000 to
0xFFFFFFF).

#### Parsing ELF Section Headers ####

The first order of business is to fill out those page tables with correct
present, read/write, and physical address information. At this point in the
initializing process no memory has been dynamically allocated, so this just
means walking the ELF header information about the kernel binary and setting
up the page tables to reflect what was already put in memory by the boot
loader.

### Physical Memory Management ###

Once the kernel's page tables are correct, then we can dynamically allocate
the memory for a physical memory manager. Which will handle which physical
frames are free, etc.

TODO: Describe the bootstrapping process, and the types involved.