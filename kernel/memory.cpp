#include "kernel/memory.h"

#include "klib/debug.h"

namespace {

const uint32 kPDEAddressMask = 0b11111111111111111111000000000000;
const uint32 k4KiBMask       = 0b00000000000000000000111111111111;

}  // anonymous namespace

namespace kernel {

PageDirectoryEntry::PageDirectoryEntry() : data_(0) {}

uint32 PageDirectoryEntry::GetPageTableAddress() {
  uint32 address = data_ & kPDEAddressMask;
  return address;
}

void PageDirectoryEntry::SetPageTableAddress(uint32 page_table_address) {
  // Ensure 4KiB-aligned.
  page_table_address &= kPDEAddressMask;
  // Zero out the old address.
  data_ &= ~kPDEAddressMask;
  // Put in the new address.
  data_ |= page_table_address;
}

#define BIT_FLAG_GETTER(name, bit)          \
bool PageDirectoryEntry::Get##name##Bit() { \
  uint32 mask = 1;                          \
  mask <<= bit;                             \
  mask &= data_;                            \
  return mask;                              \
}

#define BIT_FLAG_SETTER(name, bit)                \
void PageDirectoryEntry::Set##name##Bit(bool f) { \
  uint32 mask = 1;                                \
  mask <<= bit;                                   \
  data_ &= ~mask;                                 \
  if (f) {                                        \
    data_ |= mask;                                \
  }                                               \
}

// Yes, macros calling macros. Deal with it.
#define BIT_FLAG_MEMBER(name, bit) \
  BIT_FLAG_GETTER(name, bit)       \
  BIT_FLAG_SETTER(name, bit)

BIT_FLAG_MEMBER(Present,      0)
BIT_FLAG_MEMBER(ReadWrite,    1)
BIT_FLAG_MEMBER(User,         2)
BIT_FLAG_MEMBER(WriteThrough, 3)
BIT_FLAG_MEMBER(DisableCache, 4)
BIT_FLAG_MEMBER(Dirty,        5)
BIT_FLAG_MEMBER(Size,         7)

#undef BIT_FLAG_GETTER
#undef BIT_FLAG_SETTER
#undef BIT_FLAG_MEMBERS

}  // namespace kernel
