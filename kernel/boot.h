// GRUB-related info.

#ifndef KERNEL_GRUB_H_
#define KERNEL_GRUB_H_

#include "kernel/elf.h"
#include "klib/types.h"

namespace kernel {

namespace grub {

// Parse memory that is laid out from GRUB.
// http://wiki.osdev.org/Detecting_Memory_%28x86%29#Memory_Map_Via_GRUB
struct aout_symbol_table {
  uint32 tabsize;
  uint32 strsize;
  uint32 addr;
  uint32 reserved;
};

// The structure for this header is somewhat documented at:
// http://www.gnu.org/software/grub/manual/multiboot/html_node/Boot-information-format.html#Boot-information-format
struct multiboot_info {
  uint32 flags;
  uint32 mem_lower;
  uint32 mem_upper;
  uint32 boot_device;
  uint32 cmdline;
  uint32 mods_count;
  uint32 mods_addr;
  union {
    aout_symbol_table aout_sym;
    kernel::elf::ElfSectionHeaderTable elf_sec;
  } u;
  uint32 mmap_length;
  uint32 mmap_addr;
};

struct multiboot_memory_map {
  uint32 size;
  uint32 base_addr_low, base_addr_high;
  uint32 length_low, length_high;
  uint32 type;
};

}  // namespace grub

// Register the multiboot header information from the boot loader.
void SetMultibootInfo(const grub::multiboot_info* boot_info);
const grub::multiboot_info* GetMultibootInfo();

}  // namespace kernel

#endif  // KERNEL_GRUB_H_
