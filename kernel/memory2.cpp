#include "kernel/memory2.h"

#include "kernel/boot.h"
#include "kernel/memory.h"
#include "klib/debug.h"
#include "sys/control_registers.h"

using klib::Assert;

namespace {

// Kernel page directory table, containing a mapping for all 4GiB of addressable
// memory. 
kernel::PageDirectoryEntry kernel_page_directory_table[1024] __attribute__((aligned(4096)));
// Kernel page tables, containing entries for the top 1GiB of memory.
// 0xC0000000 - 0xFFFFFFFF.
kernel::PageTableEntry kernel_page_tables[256][1024] __attribute__((aligned(4096)));

kernel::PageFrameManager page_frame_manager;

bool IsInKernelSpace(uint32 raw_address) {
  return (raw_address > 0xC0000000);
}

// Utilities for converting between physical and virtual addresses. These ONLY
// for static memory in kernel code, which is set in place by the linker script
// (rebasing the ELF binary to 0xC0100000) and boot loader (loading the binary
// at 0x00100000). You will be in a world of pain if you use these methods for
// anything else.

uint32 ConvertPhysicalAddressToVirtual(uint32 raw_address) {
  return (raw_address + 0xC0000000);
}

uint32 ConvertVirtualAddressToPhysical(uint32 raw_address) {
  return (raw_address - 0xC0000000);
}

// TODO(chris): Clean this up and export, as it will come in handy later.
/*
void DumpKernelMemory() {
  uint32 current_pdt = get_cr3());
  uint32 kernel_pdt = GetPhysicalAddress((uint32) kernel_page_directory_table);

  uint32* current_pdt_ptr = (uint32*) current_pdt;

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
*/

}  // anonymous namespace

namespace kernel {

void InitializeKernelPageDirectory() {
  klib::Debug::Log("Initializing kernel page directory.");
  // TODO(chrsmith): Memset to zero out the PDT and PTs, just to be sure.

  // We only initilize Page Directory Table entries > 768 (0xC0000000).
  for (size pdte = 768; pdte < 1024; pdte++) {
    kernel_page_directory_table[pdte].SetPresentBit(true);
    kernel_page_directory_table[pdte].SetReadWriteBit(true);
    kernel_page_directory_table[pdte].SetUserBit(false);
    kernel_page_directory_table[pdte].SetAddress(
        ConvertVirtualAddressToPhysical((uint32) kernel_page_tables[pdte - 768]));
  }

  // Identity map the first MiB of memory to 0xC0000000. This way we can access
  // hardware registers (e.g. TextUI memory) from the kernel.
  for (size page_table = 0; page_table < 256; page_table++) {
    for (size pte = 0; pte < 1024; pte++) {
      kernel_page_tables[page_table][pte].SetPresentBit(true);
      kernel_page_tables[page_table][pte].SetReadWriteBit(true);
      kernel_page_tables[page_table][pte].SetUserBit(false);

      uint32 physical_address = page_table * 4 * 1024 * 1024 + pte * 4096;
      kernel_page_tables[page_table][pte].SetAddress(physical_address);
    }
  }

  // Initialize page tables marking kernel code that has already been
  // loaded into memory. This assumes that the boot loader put the entire ELF
  // binary in memory starting at the 1MiB mark. We assume that the kernel page
  // tables are ordered correctly, so it is just a matter of mapping the virtual
  // addresses used by the loaded ELF code to the physical memory address it is
  // loaded into. i.e. virt addr X maps to physical address (X - 0xC0000000).
  //
  // Note that the boot loader puts the multiboot data structures below the 1MiB
  // mark, which means we need to convert pointers to the virtualized ones via
  // the basic paging set up before C-code began executing.
  const grub::multiboot_info* boot_info = (grub::multiboot_info*)
      ConvertPhysicalAddressToVirtual((uint32) GetMultibootInfo());

  const kernel::elf::ElfSectionHeaderTable* elf_sht = &(boot_info->u.elf_sec);
  Assert((boot_info->flags & 0b100000) != 0, "Multiboot flags not set.");
  Assert(sizeof(kernel::elf::Elf32SectionHeader) == elf_sht->size,
         "ELF section header doesn't match expected size.");

  for (size section_idx = 0; section_idx < size(elf_sht->num); section_idx++) {
    uint32 section_header_address = ConvertPhysicalAddressToVirtual(elf_sht->addr);
    const kernel::elf::Elf32SectionHeader* section =
        kernel::elf::GetSectionHeader(section_header_address, section_idx);

    uint32 section_physaddr = section->addr;
    // Undo linker schenanigans to load the ELF binary in high-memory.
    if (IsInKernelSpace(section_physaddr)) {
      section_physaddr = ConvertVirtualAddressToPhysical(section_physaddr);
    }

    // We know where the data exists in physical memory, which page table do we
    // put the mapping into?
    size page_table = section_physaddr / 4 * 1024 * 1024;
    size starting_page_table_entry = (section_physaddr % 4 * 1024 * 1024) / 4096;


    // The following code is probably wrong, but gets the right result in the
    // end. ELF sections are not guaranteed to be page-aligned. e.g. .text
    // appears to be broken up into multiple sections (".text._ZN3hal6Te").
    // Because of this multiple sections can overlap in the same page. We simply
    // re-initialize the page tale entry with the same settings. In theory, the
    // only time this will be a problem if two ELF sections sharing a page need
    // different page settings. (One section is read-only the other is
    // read-write.)
    size section_pages = (section->size / 4096);
    section_pages += (section->size % 4096 == 0) ? 0 : 1;  // For rounding.
    for (size page_to_map = 0; page_to_map < section_pages; page_to_map++) {
      // NOTE: For ELF sections that are larger than 4MiB this should "just work".
      // The page table entry will be > 1024, and is part of another page table,
      // but the way the kernel_page_tables are laid out in memory it will write
      // to the correct address.
      size pte = starting_page_table_entry + page_to_map;
      uint32 page_physaddr = page_table * 4 * 1024 * 1024 + pte * 4096;

      bool is_writeable = (section->flags & 0x1);

      kernel_page_tables[page_table][pte].SetPresentBit(true);
      kernel_page_tables[page_table][pte].SetUserBit(false);
      kernel_page_tables[page_table][pte].SetReadWriteBit(is_writeable);
      kernel_page_tables[page_table][pte].SetAddress(page_physaddr);
    }
  }
  
  set_cr3(ConvertVirtualAddressToPhysical((uint32) kernel_page_directory_table));
  klib::Debug::Log("  Kernel page directory table loaded.");
}

void InitializePageFrameManager() {
  klib::Debug::Log("Initializing page frame manager");

  size num_regions = 0;
  MemoryRegion regions[32];

  // Determine the usable memory regions.
  const kernel::grub::multiboot_info* mbt =
      (kernel::grub::multiboot_info*) ConvertPhysicalAddressToVirtual(
          (uint32) kernel::GetMultibootInfo());

  kernel::grub::multiboot_memory_map* mmap =
      (kernel::grub::multiboot_memory_map*) ConvertPhysicalAddressToVirtual(
          mbt->mmap_addr);

  uint32 last_region_end = 0;
  while((uint32) mmap < (uint32) ConvertPhysicalAddressToVirtual(mbt->mmap_addr) +
                        mbt->mmap_length) {
    const uint32 region_start = mmap->base_addr_low;
    const uint32 region_end = mmap->base_addr_low + mmap->length_low;

    // This is not expected according to APIC spec. But assumed for now.
    // TODO(chris): Handle this case.
    if (region_start < last_region_end) {
      klib::Panic("Memory map regions not sorted.");
    }
    last_region_end = region_end;

    if (mmap->type == 1 || mmap->type == 3) {
      regions[num_regions].address = region_start;
      regions[num_regions].size = mmap->length_low;
      num_regions++;
    }

    mmap = (kernel::grub::multiboot_memory_map*) (
        (uint32) mmap + mmap->size + sizeof(uint32));
  }

  klib::Debug::Log("  Found %d usable memory regions.", num_regions);
  page_frame_manager.Initialize(regions, num_regions);
  klib::Debug::Log("  page_frame_manager.NumFrames() = %d",
                   page_frame_manager.NumFrames());
}

void SyncPhysicalAndVirtualMemory() {
  klib::Debug::Log("Syncing Physical and Virtual memory");
  klib::Debug::Log("  %d reserved page frames before.",
                   page_frame_manager.ReservedFrames());
  // Only scan kernel page directory entries.
  for (size pde = 0; pde < 256; pde++) {
    for (size pte = 0; pte < 1024; pte++) {
      if (kernel_page_tables[pde][pte].PresentBit()) {
        MemoryError err = page_frame_manager.ReserveFrame(
            kernel_page_tables[pde][pte].Address());
        if (err != MemoryError::NoError) {
          klib::Debug::Log("Got MemoryError[%d] after reserving address %h at pde %d pte %d",
                           (uint32) err, kernel_page_tables[pde][pte].Address(), pde, pte);
        }
        Assert(err == MemoryError::NoError);
      }
    }
  }

  klib::Debug::Log("  %d reserved page frames after.",
                   page_frame_manager.ReservedFrames());
}

}  // namespace kernel
