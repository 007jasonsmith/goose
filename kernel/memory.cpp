#include "kernel/memory.h"

#include "kernel/boot.h"
#include "klib/debug.h"

using klib::Assert;

namespace {

const uint32 kAddressMask = 0b11111111111111111111000000000000;
const uint32 k4KiBMask    = 0b00000000000000000000111111111111;

// Kernel page directory table, containing a mapping for all 4GiB of addressable
// memory. 
kernel::PageDirectoryEntry kernelPageDirectoryTable[1024] __attribute__((aligned(4096)));
// Kernel page tables, containing entries for the top 1GiB of memory.
// 0xC0000000 - 0xFFFFFFFF.
kernel::PageTableEntry kernelPageTables[256 * 1024] __attribute__((aligned(4096)));

bool IsInKernelSpace(uint32 raw_address) {
  return (raw_address > 0xC0000000);
}

// Virtualize a pointer. Since GRUB's addresses are to _physical_
// memory address, and by the time the kernel loads virtual memory
// has been enabled, we need to be careful about accessing it.
uint32 VirtualizeAddress(uint32 raw_address) {
  return (raw_address + 0xC0000000);
}

bool Is4KiBAligned(uint32 address) {
  return (address % 4096 == 0);
}

}  // anonymous namespace

namespace kernel {

#define BIT_FLAG_GETTER(clsname, name, bit) \
bool clsname::Get##name##Bit() {            \
  uint32 mask = 1;                          \
  mask <<= bit;                             \
  mask &= data_;                            \
  return mask;                              \
}

#define BIT_FLAG_SETTER(clsname, name, bit) \
void clsname::Set##name##Bit(bool f) {      \
  uint32 mask = 1;                          \
  mask <<= bit;                             \
  data_ &= ~mask;                           \
  if (f) {                                  \
    data_ |= mask;                          \
  }                                         \
}

// Yes, macros calling macros. Deal with it.
#define BIT_FLAG_MEMBER(clsname, name, bit) \
  BIT_FLAG_GETTER(clsname, name, bit)       \
  BIT_FLAG_SETTER(clsname, name, bit)

PageDirectoryEntry::PageDirectoryEntry() : data_(0) {}

uint32 PageDirectoryEntry::GetPageTableAddress() {
  uint32 address = data_ & kAddressMask;
  return address;
}

void PageDirectoryEntry::SetPageTableAddress(uint32 page_table_address) {
  // Ensure 4KiB-aligned.
  page_table_address &= kAddressMask;
  // Zero out the old address.
  data_ &= ~kAddressMask;
  // Put in the new address.
  data_ |= page_table_address;
}

BIT_FLAG_MEMBER(PageDirectoryEntry, Present,      0)
BIT_FLAG_MEMBER(PageDirectoryEntry, ReadWrite,    1)
BIT_FLAG_MEMBER(PageDirectoryEntry, User,         2)
BIT_FLAG_MEMBER(PageDirectoryEntry, WriteThrough, 3)
BIT_FLAG_MEMBER(PageDirectoryEntry, DisableCache, 4)
BIT_FLAG_MEMBER(PageDirectoryEntry, Accessed,     5)
BIT_FLAG_MEMBER(PageDirectoryEntry, Size,         7)

PageTableEntry::PageTableEntry() : data_(0) {}

uint32 PageTableEntry::GetPhysicalAddress() {
  uint32 address = data_ & kAddressMask;
  return address;
}

void PageTableEntry::SetPhysicalAddress(uint32 physical_address) {
  // Ensure 4KiB-aligned.
  physical_address &= kAddressMask;
  // Zero out the old address.
  data_ &= ~kAddressMask;
  // Put in the new address.
  data_ |= physical_address;
}

BIT_FLAG_MEMBER(PageTableEntry, Present,      0)
BIT_FLAG_MEMBER(PageTableEntry, ReadWrite,    1)
BIT_FLAG_MEMBER(PageTableEntry, User,         2)
BIT_FLAG_MEMBER(PageTableEntry, WriteThrough, 3)
BIT_FLAG_MEMBER(PageTableEntry, DisableCache, 4)
BIT_FLAG_MEMBER(PageTableEntry, Accessed,     5)
BIT_FLAG_MEMBER(PageTableEntry, Dirty,        6)
BIT_FLAG_MEMBER(PageTableEntry, Global,       8)

#undef BIT_FLAG_GETTER
#undef BIT_FLAG_SETTER
#undef BIT_FLAG_MEMBERS

void InitializeKernelPageDirectory() {
  // Initialize page directory tables.
  for (size i = 0; i < 1024; i++) {
    kernelPageDirectoryTable[i].SetPresentBit(false);
    
    // Map addresses > 0xC0000000 to kernel page tables.
    if (i >= 768) {
      kernelPageDirectoryTable[i].SetPresentBit(true);
      kernelPageDirectoryTable[i].SetReadWriteBit(true);
      kernelPageDirectoryTable[i].SetPageTableAddress(
          (uint32) (&kernelPageTables[i * 1024]));
    }
  }

  // Initialize page tables. Zero out for now, will map to the ELF
  // binary info next.
  for (size i = 0; i < 256 * 1024; i++) {
    kernelPageTables[i].SetPresentBit(false);
  }

  // WHERE IS THE MULTIBOOTINFO LOCATED?

  // Initialize page tables marking kernel code that has already been
  // loaded into memory. This section assumes that GRUB loads the entire
  // ELF binary starting at the 1MiB mark, and that we specify the starting
  // address at 0xC0100000 (0xC0000000 + 1MiB).
  const grub::multiboot_info* multiboot_info = GetMultibootInfo();
  const kernel::elf::ElfSectionHeaderTable* elf_ht =
      &(multiboot_info->u.elf_sec);
  Assert((multiboot_info->flags & 0b100000) != 0,
         "Multiboot flags bit not set.");
  Assert(sizeof(kernel::elf::Elf32SectionHeader) != elf_ht->size,
         "ELF section header doesn't match expected size.");
  Assert(IsInKernelSpace(elf_ht->addr), "Expected ELF header in kernel space.");

  for (size i = 0; i < size(elf_ht->num); i++) {
    const kernel::elf::Elf32SectionHeader* section =
        kernel::elf::GetSectionHeader(VirtualizeAddress(elf_ht->addr), i);
    Assert(Is4KiBAligned(section->addr), "Section is not 4KiB aligned.");
  }
  
  // Replace page directory table.

  // SHOULD WORK JUST FINE. VERIFY IT GOOD.

  // Initialize the specific kernel-level pages.
  // Map 0xC0000000-0xC0100000 to 0x00000000-0x00100000
  //     Used for low-level DMA jazz, like TextUI.
  // Map 0xC0100000-<ker size> to 0x00100000-<ker size>
  //     Used for the kernel's code. GRUB's multiboot
  //     contains the ELF info.
}

}  // namespace kernel
