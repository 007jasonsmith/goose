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
      
}  // namespace kernel
