// Data structures for parsing ELF executables.
// See: http://www.skyfree.org/linux/references/ELF_Format.pdf
// Try: readelf kernel.elf --headers
#ifndef KERNEL_ELF_H_
#define KERNEL_ELF_H_

#include "klib/panic.h"
#include "klib/types.h"

namespace kernel {
namespace elf {

// All values below assume a 32-bit ELF header.

enum class SectionType : uint32 {
  // Marks the section header as inactive.
  NULL =  0,
  // Information defined by the program.
  PROGBITS = 1,
    // Symbol table.
  SYMTAB   = 2,
    // String table.
  STRTAB   = 3,
    // Relocation entries with explicit addends.
  RELA     = 4,
    // Symbol table hash.
  HASH     = 5,
  DYNAMIC  = 6,
  NOTE     = 7,
  NOBITS   = 8,
  REL      = 9,
  SHLIB    = 10,
    // Symbol table
  DYNSYM   = 11,
  LOPROC   = 0x80000000,
  HIPROC   = 0x7FFFFFFF,
  LOUSER   = 0x80000000,
  HIUSER   = 0xFFFFFFFF
};

struct Elf32SectionHeader {
  uint32 name;       // Section name (index into string table)
  uint32 type;       // Section type (SHT_*)
  uint32 flags;      // Section flags (SHF_*)
  uint32 addr;       // Address where section is to be loaded
  uint32 offset;     // File offset of section data, in bytes
  uint32 size;       // Size of section, in bytes
  uint32 link;       // Section type-specific header table index link
  uint32 info;       // Section type-specific extra information
  uint32 addralign;  // Section address alignment
  uint32 entsize;    // Size of records contained within the section
};

// TODO(chris): Put this into elf.cc once it actually works.
const char* GetStringTableEntry(uint32 strtab_address, uint32 size, uint32 index) {
  const char* p = (const char*) strtab_address;
  const char* table_end = (const char*) (strtab_address + size);
  if (*p != 0) {
    klib::Panic("First entry in StringTable non-null.");
  }
  if (*table_end != 0) {
    klib::Panic("Last entry in StingTable non-null.");
  }

  const char* start = nullptr;
  uint32 current_index = 0;
  while (current_index <= index) {
    // Starting assuming p is on the null of the previous string.
    // (Or on the null of the first byte of the section.) Advance
    // one byte to next string.
    p++;
    if (p >= table_end) {
      return nullptr;
    }
    start = p;
    while (*p) {
      p++;
    }
    current_index++;
  }

  return start;
}

}  // namespace elf
}  // namespace kernel

#endif  // KERNEL_ELF_H_
