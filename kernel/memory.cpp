#include "kernel/memory.h"

using klib::Assert;

namespace {

const uint32 kAddressMask = 0b11111111111111111111000000000000;
const uint32 k4KiBMask    = 0b00000000000000000000111111111111;

}  // anonymous namespace

namespace kernel {

PointerTableEntry::PointerTableEntry() : data_(0) {}

uint32 PointerTableEntry::Address() const {
  uint32 address = data_ & kAddressMask;
  return address;
}

void PointerTableEntry::SetAddress(uint32 address) {
  // Ensure 4KiB-aligned.
  address &= kAddressMask;
  // Zero out the old address.
  data_ &= ~kAddressMask;
  // Put in the new address.
  data_ |= address;
}

uint32 PointerTableEntry::Value() const {
  return data_;
}

bool PointerTableEntry::StatusFlag(size bit) const {
  uint32 mask = 1;
  mask <<= bit;
  mask &= data_;
  return mask;
}

void PointerTableEntry::SetStatusFlag(size bit, bool value) {
  uint32 mask = 1;
  mask <<= bit;
  data_ &= ~mask;
  if (value) {
    data_ |= mask;
  }
}

#define BIT_FLAG_GETTER(clsname, name, bit) \
bool clsname::name##Bit() const {           \
  return StatusFlag(bit);                   \
}

#define BIT_FLAG_SETTER(clsname, name, bit) \
void clsname::Set##name##Bit(bool f) {      \
  SetStatusFlag(bit, f);                    \
}

// Yes, macros calling macros. Deal with it.
#define BIT_FLAG_MEMBER(clsname, name, bit) \
  BIT_FLAG_GETTER(clsname, name, bit)       \
  BIT_FLAG_SETTER(clsname, name, bit)


PageDirectoryEntry::PageDirectoryEntry() : PointerTableEntry() {}
BIT_FLAG_MEMBER(PageDirectoryEntry, Present,      0)
BIT_FLAG_MEMBER(PageDirectoryEntry, ReadWrite,    1)
BIT_FLAG_MEMBER(PageDirectoryEntry, User,         2)
BIT_FLAG_MEMBER(PageDirectoryEntry, WriteThrough, 3)
BIT_FLAG_MEMBER(PageDirectoryEntry, DisableCache, 4)
BIT_FLAG_MEMBER(PageDirectoryEntry, Accessed,     5)
BIT_FLAG_MEMBER(PageDirectoryEntry, Size,         7)

PageTableEntry::PageTableEntry() : PointerTableEntry() {}
BIT_FLAG_MEMBER(PageTableEntry, Present,      0)
BIT_FLAG_MEMBER(PageTableEntry, ReadWrite,    1)
BIT_FLAG_MEMBER(PageTableEntry, User,         2)
BIT_FLAG_MEMBER(PageTableEntry, WriteThrough, 3)
BIT_FLAG_MEMBER(PageTableEntry, DisableCache, 4)
BIT_FLAG_MEMBER(PageTableEntry, Accessed,     5)
BIT_FLAG_MEMBER(PageTableEntry, Dirty,        6)
BIT_FLAG_MEMBER(PageTableEntry, Global,       8)

FrameTableEntry::FrameTableEntry() : PointerTableEntry() {}
BIT_FLAG_MEMBER(FrameTableEntry, InUse, 0)

#undef BIT_FLAG_GETTER
#undef BIT_FLAG_SETTER
#undef BIT_FLAG_MEMBERS

}  // namespace kernel
