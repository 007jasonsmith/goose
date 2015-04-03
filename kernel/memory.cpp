#include "kernel/memory.h"

#include "klib/debug.h"

namespace {

const uint32 kAddressMask = 0b11111111111111111111000000000000;
const uint32 k4KiBMask    = 0b00000000000000000000111111111111;

// NOTE: If these were present, the kernel won't load anymore.
// I suspect since it will take > 4MiB and won't fit into the
// bootstrapped paging.
kernel::PageDirectoryEntry gPageDirectoryTable[0/*1024*/];
kernel::PageTableEntry gPageTables[0/*1024 * 1024*/];

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

void InitializeStartingPageTables() {
  // Initialize page directory tables.
  for (size i = 0; i < 1024; i++) {
    gPageDirectoryTable[i].SetPageTableAddress(
        (uint32) (&gPageTables[i * 1024]));        
    gPageDirectoryTable[i].SetPresentBit(true);  // Page table exists.
    gPageDirectoryTable[i].SetUserBit(true);
    gPageDirectoryTable[i].SetReadWriteBit(false);
  }

  // Initialize page tables.
  for (size i = 0; i < 1024 * 1024; i++) {
    gPageTables[i].SetPhysicalAddress(i * 4 * 1024);
    gPageTables[i].SetPresentBit(false);
    gPageTables[i].SetUserBit(true);
  }

  // Initialize the specific kernel-level pages.
  // Map 0xC0000000-0xC0100000 to 0x00000000-0x00100000
  //     Used for low-level DMA jazz, like TextUI.
  // Map 0xC0100000-<ker size> to 0x00100000-<ker size>
  //     Used for the kernel's code. GRUB's multiboot
  //     contains the ELF info.
}

}  // namespace kernel
