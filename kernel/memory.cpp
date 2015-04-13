#include "kernel/memory.h"

#include "kernel/boot.h"
#include "klib/debug.h"
#include "sys/control_registers.h"

using klib::Assert;

namespace {

const uint32 kAddressMask = 0b11111111111111111111000000000000;
const uint32 k4KiBMask    = 0b00000000000000000000111111111111;

// Kernel page directory table, containing a mapping for all 4GiB of addressable
// memory. 
kernel::PageDirectoryEntry kernel_page_directory_table[1024] __attribute__((aligned(4096)));
// Kernel page tables, containing entries for the top 1GiB of memory.
// 0xC0000000 - 0xFFFFFFFF.
kernel::PageTableEntry kernel_page_tables[256][1024] __attribute__((aligned(4096)));

bool IsInKernelSpace(uint32 raw_address) {
  return (raw_address > 0xC0000000);
}

// Virtualize a pointer. Since GRUB's addresses are to _physical_
// memory address, and by the time the kernel loads virtual memory
// has been enabled, we need to be careful about accessing it.
uint32 VirtualizeAddress(uint32 raw_address) {
  return (raw_address + 0xC0000000);
}

uint32 GetPhysicalAddress(uint32 address) {
  return (address - 0xC0000000);
}

template<typename T>
T* VirtualizeAddress(T* raw_addr) {
  uint32 addr = (uint32) raw_addr;
  return (T*) (addr + 0xC0000000);
}

bool Is4KiBAligned(uint32 address) {
  return (address % 4096 == 0);
}

void DumpKernelMemory() {
  uint32 current_pdt = VirtualizeAddress(get_cr3());
  uint32 kernel_pdt = GetPhysicalAddress((uint32) kernel_page_directory_table);

  uint32* current_pdt_ptr = (uint32*) current_pdt;

  // TEMP
  klib::Debug::Log("Current PDT (4MiB pages, etc.)");
  klib::Debug::Log("  Current PDT (CR3) is %h", current_pdt);
  for (size pdt = 0; pdt < 1024; pdt++) {
    kernel::PageDirectoryEntry pde = kernel::PageDirectoryEntry(current_pdt_ptr[pdt]);
    if (pde.Value() != 0) {
      klib::Debug::Log("  %{L4}d| %h %b",
		       pdt,
		       pde.GetPageTableAddress(),
		       pde.Value());
    }    
  }

  klib::Debug::Log("Kernel Page Directory Table:");
  klib::Debug::Log("  CR4 %b", get_cr4());
  klib::Debug::Log("  Kernel PDT %h, current PDT %h", kernel_pdt, current_pdt);
  for (size pdt = 0; pdt < 1024; pdt++) {
    if (kernel_page_directory_table[pdt].Value() != 0) {
      klib::Debug::Log("  %{L4}d| %h %b",
		       pdt,
		       kernel_page_directory_table[pdt].GetPageTableAddress(),
		       kernel_page_directory_table[pdt].Value());
    }
  }
  
  klib::Debug::Log("Kernel Page Tables:");
  klib::Debug::Log("  Kernel PT %h", (uint32) kernel_page_tables);
  for (size pt = 0; pt < 256; pt++) {
    for (size pte = 0; pte < 1024; pte++) {
      if (kernel_page_tables[pt][pte].Value() != 0) {
	klib::Debug::Log("  %{L3}d %{L4}d| %h %b",
			 pt, pte,
			 kernel_page_tables[pt][pte].GetPhysicalAddress(),
			 kernel_page_tables[pt][pte].Value());
      }
    }
  }
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

// DO NOT SUBMIT
PageDirectoryEntry::PageDirectoryEntry(uint32 data) : data_(data) {}

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

uint32 PageDirectoryEntry::Value() {
  return data_;
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

uint32 PageTableEntry::Value() {
  return data_;
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

// Convert a 4MiB page into using a separate page table.
void InitializeKernelPageDirectory() {
  // Page table #0, pages from 0x0 to 4MiB.
  for (size pte = 0; pte < 1024; pte++) {
    kernel_page_tables[0][pte].SetPresentBit(true);
    kernel_page_tables[0][pte].SetReadWriteBit(true);
    uint32 address = pte * 4 * 1024;
    kernel_page_tables[0][pte].SetPhysicalAddress(address);
  }
  // Second PT, 4MiB - 8MiB.
  for (size pte = 0; pte < 1024; pte++) {
    kernel_page_tables[1][pte].SetPresentBit(true);
    kernel_page_tables[1][pte].SetReadWriteBit(true);
    uint32 address = pte * 4 * 1024 + 4 * 1024 * 1024;
    kernel_page_tables[1][pte].SetPhysicalAddress(address);
  }

  // Two 4MiB pages, mapping 0xC0000000 + 8MiB to physical address
  // 0x00000000 + 8MiB.
  kernel_page_directory_table[768].SetPresentBit(true);
  kernel_page_directory_table[768].SetReadWriteBit(true);
  // kernel_page_directory_table[768].SetSizeBit(true);
  kernel_page_directory_table[768].SetPageTableAddress(
      GetPhysicalAddress((uint32) &kernel_page_tables[0]));
						      
  kernel_page_directory_table[769].SetPresentBit(true);
  kernel_page_directory_table[769].SetReadWriteBit(true);
  kernel_page_directory_table[769].SetPageTableAddress(
      GetPhysicalAddress((uint32) &kernel_page_tables[1]));

  DumpKernelMemory();
  set_cr3(GetPhysicalAddress((uint32) kernel_page_directory_table));
  // DON'T DO THIS. WILL FAIL. Get "(invalid)  : FFF" from bochslog.txt.
  // set_cr3((uint32) kernel_page_directory_table);
}

#if 0
// Since all the other attempts to set up paging have failed,
// this is just recreating the page directory table the system
// has at boot. This should work.
void InitializeKernelPageDirectoryWorks() {
  // Two 4MiB pages, mapping 0xC0000000 + 8MiB to physical address
  // 0x00000000 + 8MiB.
  kernel_page_directory_table[768].SetPresentBit(true);
  kernel_page_directory_table[768].SetReadWriteBit(true);
  kernel_page_directory_table[768].SetSizeBit(true);
  kernel_page_directory_table[768].SetPageTableAddress(0x00000000);

  kernel_page_directory_table[769].SetPresentBit(true);
  kernel_page_directory_table[769].SetReadWriteBit(true);
  kernel_page_directory_table[769].SetSizeBit(true);
  kernel_page_directory_table[769].SetPageTableAddress(0x400000);

  DumpKernelMemory();
  set_cr3(GetPhysicalAddress((uint32) kernel_page_directory_table));
  // DON'T DO THIS. WILL FAIL. Get "(invalid)  : FFF" from bochslog.txt.
  // set_cr3((uint32) kernel_page_directory_table);
}

// Sanity check things. Just map the first 8MiB using 4KiB pages. Similar to how
// things are set up before C-code gets called.
void InitializeKernelPageDirectory() {
  // TODO(chris): memset to zero out the PDT and PTs.

  // Initialize page directory tables. Ignore everything but virtual addresses
  // >= 0xC0000000.
  for (size i = 768; i < 1024; i++) {
    kernel_page_directory_table[i].SetPresentBit(true);
    kernel_page_directory_table[i].SetReadWriteBit(true);
    kernel_page_directory_table[i].SetUserBit(true);  // Not needed?
    kernel_page_directory_table[i].SetPageTableAddress(
        (uint32) (&kernel_page_tables[i - 768]));
  }

  // For page tables at virtual addresses >= 0xC0000000, map them to physical
  // addresses starting at 0x00000000.
  for (size pt = 0; pt < 2; pt++) {
    for (size pte = 0; pte < 1024; pte++) {
      kernel_page_tables[pt][pte].SetPresentBit(true);
      kernel_page_tables[pt][pte].SetReadWriteBit(true);
      kernel_page_tables[pt][pte].SetUserBit(true);  // Not needed?
      uint32 address = pt * 4 * 1024 * 1024 + pte * 4 * 1024;
      kernel_page_tables[pt][pte].SetPhysicalAddress(GetPhysicalAddress(address));
    }
  }

  DumpKernelMemory();
  set_cr3(GetPhysicalAddress((uint32) kernel_page_directory_table));
}

void InitializeKernelPageDirectoryButNotWork() {
  // Initialize page directory tables.
  for (size i = 0; i < 1024; i++) {
    kernel_page_directory_table[i].SetPresentBit(false);
    
    // Map addresses > 0xC0000000 to kernel page tables.
    if (i >= 768) {
      kernel_page_directory_table[i].SetPresentBit(true);
      kernel_page_directory_table[i].SetReadWriteBit(true);
      kernel_page_directory_table[i].SetPageTableAddress(
          (uint32) (&kernel_page_table[(i - 768) * 1024]));
    }
  }

  // Initialize page tables. Zero out for now, will map to the ELF
  // binary info next.
  for (size i = 0; i < 256 * 1024; i++) {
    kernel_page_table[i].SetPresentBit(false);
  }

  // Idenity map the first MiB of memory to 0xC0000000. This way we can access
  // hardware registers (e.g. TextUI memory).
  for (size page = 0; page < 256; page++) {
      kernel_page_table[page].SetPresentBit(true);
      kernel_page_table[page].SetUserBit(false);
      kernel_page_table[page].SetReadWriteBit(true);
      kernel_page_table[page].SetPhysicalAddress(page * 4096);
  }

  // TODO(chrsmith): Figure out where GRUB puts the multiboot info pointer.
  // It looks like it is in the middle of the kernel's .bss segment.

  // Initialize page tables marking kernel code that has already been
  // loaded into memory. This section assumes that GRUB loads the entire
  // ELF binary starting at the 1MiB mark, and that we specify the starting
  // address at 0xC0100000 (0xC0000000 + 1MiB).
  const grub::multiboot_info* multiboot_info = VirtualizeAddress(GetMultibootInfo());
  klib::Debug::Log("ElfSectionHeaderTable is at %h",
                   (uint32) &(multiboot_info->u.elf_sec));
  const kernel::elf::ElfSectionHeaderTable* elf_sht =
      &(multiboot_info->u.elf_sec);
  Assert((multiboot_info->flags & 0b100000) != 0,
         "Multiboot flags bit not set.");
  klib::Debug::Log("%d and %d", sizeof(kernel::elf::Elf32SectionHeader), elf_sht->size);
  Assert(sizeof(kernel::elf::Elf32SectionHeader) == elf_sht->size,
         "ELF section header doesn't match expected size.");
  klib::Debug::Log("Setting up page tables for %d ELF header table sections.",
                   elf_sht->num);
  for (size i = 0; i < size(elf_sht->num); i++) {
    const kernel::elf::Elf32SectionHeader* section =
        kernel::elf::GetSectionHeader(VirtualizeAddress(elf_sht->addr), i);

    // The following code is definitely wrong, but gets the right result in the
    // end. ELF sections are not guaranteed to be page-aligned. e.g. .text
    // (appears) to be broken up into multiple sections (".text._ZN3hal6Te").
    // Because of this multiple sections can overlap in the same page. We simply
    // re-initialize the page tale entry with the same settings. In theory, the
    // only time this will be a problem if two ELF sections sharing a page need
    // different page settings. (read-only/read-write, etc.)

    // BUG: If section size is exactly 4KiB aligned, we waste 1 page.
    size section_pages = (section->size / 4096) + 1;
    for (size page_to_map = 0; page_to_map < section_pages; page_to_map++) {
      klib::Debug::Log("Mapping %{L2}d of %{L2}d pages for ELF section %d",
                       page_to_map, section_pages, i);
      uint32 section_address = section->addr;
      klib::Debug::Log("  Section address is at %h", section->addr);

      // GRUB still loads ELF sections that shouldn't typically be allocated,
      // e.g. debug symbols. However it doesn't set the virtualized address. So
      // manually add 0xC0000000 to these addresses.
      if (!IsInKernelSpace(section_address)) {
	section_address += 0xC0000000;
        klib::Debug::Log("  Section is not in kernel space, so adding 0xC0000000...");
      }

      uint32 page_virtual_address = section_address + page_to_map * 4096;
      size page_index = (page_virtual_address - 0xC0000000) / 4096;
      klib::Debug::Log("  Initializing page table at index %d", page_index);
      kernel_page_table[page_index].SetPresentBit(true);
      kernel_page_table[page_index].SetUserBit(false);
      // TODO: Check section flags.
      kernel_page_table[page_index].SetReadWriteBit(true);
      kernel_page_table[page_index].SetPhysicalAddress(
          page_virtual_address - 0xC0000000);
    }
  }
  
  // Replace page directory table.
  DumpKernelMemory();
  set_cr3(GetPhysicalAddress((uint32) kernel_page_directory_table));  // ???

  // SHOULD WORK JUST FINE. VERIFY IT GOOD.

  // Initialize the specific kernel-level pages.
  // Map 0xC0000000-0xC0100000 to 0x00000000-0x00100000
  //     Used for low-level DMA jazz, like TextUI.
  // Map 0xC0100000-<ker size> to 0x00100000-<ker size>
  //     Used for the kernel's code. GRUB's multiboot
  //     contains the ELF info.
}
#endif  // #if 0
}  // namespace kernel
