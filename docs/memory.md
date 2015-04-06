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

Kernel is linked such that pointers are relative to the address 0xC0100000.
That is, 3GiB + 1MiB. We then load the kernel into physical memory 0x00100000,
that is the 1MiB mark. This is necessary since it is possible the machine we are
running Goose on doesn't have 3GiB anyways.

Now, we immediately have a problem. If we set the instruction pointer to
~0x00100000 to start executing the kernel's code, it will immediately try to
access pointers to memory that doesn't exist. (Above the 3GiB mark.)

So in order to actually boot, we need to enable virtual memory / paging.

- For simplicty we start with 4MiB pages. We map the first 4MiB and the first
  4MiB starting at the 3GiB mark. Both point to memory address 0.
- Once we jump to the 0xC0100000 mark and start executing code, we are
  bootstrapped and no longer need the first page. (Remember, the kernel's code
  exists in the __physical__ memory at that location. But we no longer need
  memory address 0x00100000 to refer to the kernel, since 0xC0100000 will do
  the trick.

Note however that this does mean that in order to access any data returned from
GRUB, you need to "virtualize" the memory address. In other words, add 0xC0000000
too it. Since GRUB loads data starting at the 1MiB mark (which is where our
linker script asks it to be put.)

#Physical Memory vs. Virtual Memory#

The computer's limited physical memory needs to be tracked. While a process
can address up to 3GiB (with 1GiB reserved for kernel code), some virtual
addresses can be swaped to disk.

Book keeping for which pages are "present" or not is handled by the page table,
but the kernel needs to know how to respond to the page fault. i.e. where to
load that memory from disk. Or, how to update the page table of another process
whose page got evicted.

##Setting up Heapspace##

- Initialize PDT
- Initialize PT
- Replace PDT
- InitPhysicalMemoryManager
- - Count physical pages
- - Sub pages used for kernel code
- - Detect PMM space needed
- - For all pages for PMM
- - - Find first free physical page not used by kernel
- - - Set up PT entry
- - Allocate PMM @ virtual address 0, spanning pages.
- - Walk KPT and init PMM (with ELF AND PMM)
