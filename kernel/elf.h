// Data structures for parsing ELF executables.
// See: http://docs.oracle.com/cd/E23824_01/html/819-0690/chapter6-94076.html
// See: [OUTDATED] http://www.skyfree.org/linux/references/ELF_Format.pdf
// Try: readelf kernel.elf --sections
#ifndef KERNEL_ELF_H_
#define KERNEL_ELF_H_

#include "klib/panic.h"
#include "klib/types.h"

namespace kernel {
namespace elf {

// All values below assume a 32-bit ELF header.

// Some sections of the ELF header are reserved, and will never
// have a Section associated with them. These are the indicies of
// the reserved indicies.
// TODO(chris): Define the enum, wire it into parsing code, etc.

struct ElfSectionHeaderTable {
  uint32 num;
  uint32 size;
  uint32 addr;
  uint32 shndx;
};

// BUG: This enum was generated from an older spec!
enum class SectionType : uint32 {
  NULL     = 0,
  PROGBITS = 1,
  SYMTAB   = 2,
  STRTAB   = 3,
  RELA     = 4,
  HASH     = 5,
  DYNAMIC  = 6,
  NOTE     = 7,
  NOBITS   = 8,
  REL      = 9,
  SHLIB    = 10,
  DYNSYM   = 11,
  INIT_ARRAY = 14,
  LOPROC   = 0x70000000,
  HIPROC   = 0x7FFFFFFF,
  LOUSER   = 0x80000000,
  HIUSER   = 0xFFFFFFFF
};

const char* ToString(SectionType type);

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

const Elf32SectionHeader* GetSectionHeader(uint32 base_address, uint32 index);

// Return the given index from the string table entry. Panics if the index
// is beyond the number of strings in the table.
const char* GetStringTableEntry(uint32 strtab_address, uint32 size, uint32 index);

}  // namespace elf
}  // namespace kernel

#endif  // KERNEL_ELF_H_
