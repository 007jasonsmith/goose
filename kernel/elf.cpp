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
  // TODO(chris): Bring this up to date.
  case SectionType::INIT_ARRAY: return "INIT_ARRAY";
  }
  return "UNKNOWN";
}

const Elf32SectionHeader* GetSectionHeader(uint32 base_address, uint32 index) {
  return (kernel::elf::Elf32SectionHeader*) (
     base_address + sizeof(kernel::elf::Elf32SectionHeader) * index);
}

const char* GetStringTableEntry(uint32 strtab_address, uint32 size, uint32 index) {
  const char* start = (const char*) strtab_address;
  if (*start != 0) {
    klib::Panic("First entry in StringTable non-null.");
  }
  const char* end = (const char*) (strtab_address + size);
  if (*end != 0) {
    klib::Panic("Last entry in StingTable non-null.");
  }
  if (index > size) {
    klib::Panic("Retrieving string starting beyond size of string table.");
  }
  return (start + index);
}

}  // namespace elf
}  // namespace kernel
