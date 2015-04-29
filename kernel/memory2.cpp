#include "kernel/memory2.h"

#include "kernel/boot.h"
#include "kernel/memory.h"
#include "klib/debug.h"
#include "sys/control_registers.h"

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

  // Initilize kernel-space page directory entries. (> 768 is 0xC0000000.)
  for (size pdte = 0; pdte < 768; pdte++) {
    kernel_page_directory_table[pdte].SetPresentBit(false);
  }
  for (size pdte = 768; pdte < 1024; pdte++) {
    kernel_page_directory_table[pdte].SetPresentBit(true);
    kernel_page_directory_table[pdte].SetReadWriteBit(true);
    kernel_page_directory_table[pdte].SetUserBit(false);
    kernel_page_directory_table[pdte].SetAddress(
        ConvertVirtualAddressToPhysical((uint32) kernel_page_tables[pdte - 768]));
  }

  // Identity map the first MiB of memory to 0xC0000000. This way we can access
  // hardware registers (e.g. TextUI memory) from the kernel.
  for (size pte = 0; pte < 256; pte++) {
    kernel_page_tables[0][pte].SetPresentBit(true);
    kernel_page_tables[0][pte].SetReadWriteBit(true);
    kernel_page_tables[0][pte].SetUserBit(false);
    kernel_page_tables[0][pte].SetAddress(pte * 4096U);
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
  Assert((boot_info->flags & 0b100000) != 0);
  Assert(sizeof(kernel::elf::Elf32SectionHeader) == elf_sht->size);

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
    size page_table = section_physaddr / (4 * 1024 * 1024);
    size starting_page_table_entry = (section_physaddr % (4 * 1024 * 1024)) / 4096;


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
	// The first 1MiB contains reserved regions, that are referenced but
	// not set as usable memory.
	if (!(pde == 0 && pte <= 256)) {
	  if (err != MemoryError::NoError) {
	    klib::Debug::Log("Got MemoryError: %s", ToString(err));
	    klib::Debug::Log("While reserving address %h at pde %d pte %d",
			     kernel_page_tables[pde][pte].Address(), pde, pte);
	  }
	  Assert(err == MemoryError::NoError);
	}
      }
    }
  }

  klib::Debug::Log("  %d reserved page frames after.",
                   page_frame_manager.ReservedFrames());
}

// TODO(chris): IMPORTANT. We aren't keeping track of who/what has
// ownership of each page. (e.g. a PID).

MemoryError AllocateKernelPage(uint32* out_address, size pages) {
  // TODO(chris): Support more pages at a time...
  Assert(pages > 0 && pages < 128);

  // TODO(chris): Check that we have enough addressable memory in the
  // kernel page tables. For now we just panic on error.
  // TODO(chris): Loop around and check again, hopefully things don't get
  // fragmented, etc.

  // The last location of virtual memory used. Incremented every time
  // the function is called. We start at page directory entry *768*, so that
  // any memory returned by this function will be usable even if executing
  // in another process's address space. (Banking on the kernel being loaded
  // into the higher-half of memory.)
  static size pde_index = 768;
  static size pt_index = 0;
  bool found_free_address_range = false;
  for (; pde_index < 1024; pde_index++) {
    // If the page directory table is present, then walk through and find
    // the first range of free pages.
    if (kernel_page_directory_table[pt_index].PresentBit()) {
      size streak_of_free_pages = 0;
      for (pt_index = 0; pt_index < 1024 - (pages - 1); pt_index++) {
	bool free = kernel_page_tables[pde_index - 768][pt_index].PresentBit();
	if (free) {
	  streak_of_free_pages++;
	} else {
	  streak_of_free_pages = 0;
	}
	if (streak_of_free_pages == pages) {
	  pt_index = pt_index - (pages - 1);  // Point to starting free PTE.
	  found_free_address_range = true;
	  break;
	}
      }	
    }

    // The page directory table entry does not exist, so initialize it.
    // ...
    if (!kernel_page_directory_table[pt_index].PresentBit()) {
      // TODO(chris): Implement, and unify code with earlier PDT/PT init code.
      klib::Panic("TODO: Initialize Page Directory Table Entry.");
    }

    // Exit look case.
    if (found_free_address_range) {
      break;
    }
  }
  if (pde_index >= 1024) {
    klib::Panic("Out of addressable memory in kernel space.");
  }

  // With the free address range (pde_index, pt_index), we now need to set up
  // the page table entries.
  for (int page_idx = 0; page_idx < pages; page_idx++) {
    uint32 page_frame_address = 0;
    MemoryError err = page_frame_manager.RequestFrame(&page_frame_address);
    // TODO(chrsmith): Handle it.
    Assert(err == MemoryError::NoError);

    PageTableEntry* pte = &kernel_page_tables[pde_index - 768][pt_index];
    Assert(pte->PresentBit() == false);
    pte->SetPresentBit(true);
    pte->SetReadWriteBit(true);
    pte->SetUserBit(false);
    pte->SetAddress(page_frame_address);
  }

  // TODO(chrsmith): Create conversion function.
  *out_address = (uint32) pde_index * 4 * 1024 * 1024 + (uint32) pt_index * 4 * 1024;

  return MemoryError::NoError;
}

MemoryError FreeKernelPage(uint32 starting_page_address, size pages) {
  // TODO(chris): Support more pages at a time...
  Assert(pages > 0 && pages < 128);
  Assert(IsInKernelSpace(starting_page_address));
  // TODO(chrsmith): Create conversion function.
  size pde_index = starting_page_address / (4 * 1024 * 1024);
  Assert(pde_index >= 768);

  // TODO(chris): If freed all PTEs in a PDE, then remove the PDE present bit.
  size pt_index = (starting_page_address % (4 * 1024 * 1024)) / (4 * 1024);
  for (size page = 0; page < pages; page++) {
    PageTableEntry* pte = &kernel_page_tables[pde_index - 768][pt_index + page];

    Assert(pte->PresentBit());
    pte->SetPresentBit(false);

    // TODO(chris): Assert(page_frame_manager.FrameInUse(pte->GetAddress()));
    page_frame_manager.FreeFrame(pte->Address());
  }

  return MemoryError::NoError;
}

}  // namespace kernel
