#include "kernel/elf.h"

namespace kernel {
namespace elf {

const char* ToString(SectionType type) {
  switch (type) {
  case SectionType::NULL:     return "NULL";
  case SectionType::PROGBITS: return "PROGBITS";
  case SectionType::SYMTAB:   return "SYMTAB";
  case SectionType::STRTAB:   return "STRTAB";
  case SectionType::RELA:     return "RELA";
  case SectionType::HASH:     return "HASH";
  case SectionType::DYNAMIC:  return "DYNAMIC";
  case SectionType::NOTE:     return "NOTE";
  case SectionType::NOBITS:   return "NOBITS";
  case SectionType::REL:      return "REL";
  case SectionType::SHLIB:    return "SHLIB";
  case SectionType::DYNSYM:   return "DYNSYM";
  case SectionType::LOPROC:   return "LOPROC";
  case SectionType::HIPROC:   return "HIPROC";
  case SectionType::LOUSER:   return "LOUSER";
  case SectionType::HIUSER:   return "HIUSER";
  }
  return "UNKNOWN";
}

const Elf32SectionHeader* GetSectionHeader(uint32 base_address, uint32 index) {
  return (kernel::elf::Elf32SectionHeader*) (
     base_address + sizeof(kernel::elf::Elf32SectionHeader) * index);
}

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
