#include "kernel/memory.h"

#include "klib/macros.h"
#include "klib/panic.h"

using klib::Assert;

namespace {

const uint32 kAddressMask = 0b11111111111111111111000000000000;
const uint32 k4KiBMask    = 0b00000000000000000000111111111111;

bool Is4KiBAligned(uint32 address) {
  return (address % 4096 == 0);
}

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

PageFrameManager::PageFrameManager() :
    next_frame_(0), num_frames_(0) {}

void PageFrameManager::Initialize(const MemoryRegion* regions,
                                  size region_count) {
  next_frame_ = 0;

  uint32 last_region_end = 0;
  for (size i = 0; i < region_count; i++) {
    const MemoryRegion* region = &regions[i];

    // Assert the memory regions are increasing in value.
    uint32 region_end = region->address + region->size;
    klib::Assert(region_end > last_region_end, "MemoryRegions not increasing.");
    last_region_end = region_end;

    uint32 aligned_address = region->address;
    uint32 region_size = region->size;
    if (!Is4KiBAligned(region->address)) {
      aligned_address += 4096 - region->address % 4096;
      region_size -= (aligned_address - region->address);
    }
    klib::Assert(Is4KiBAligned(aligned_address));
    
    size region_frames = region_size / 4096;
    for (size region_frame = 0; region_frame < region_frames; region_frame++) {
      page_frames_[num_frames_].SetAddress(
          aligned_address + region_frame * 4096);
      page_frames_[num_frames_].SetInUseBit(false);
      num_frames_++;
    }
  }
}

MemoryError PageFrameManager::RequestFrame(uint32* out_address) {  
  if (num_frames_ == 0) {
    return MemoryError::NoPageFramesAvailable;
  }

  size total_frames_checked = 0;
  while (total_frames_checked < num_frames_) {
    if (next_frame_ >= num_frames_) {
      next_frame_ = 0;
    }
    if (!page_frames_[next_frame_].InUseBit()) {
      *out_address = page_frames_[next_frame_].Address();
      page_frames_[next_frame_].SetInUseBit(true);
      next_frame_++;
      return MemoryError::NoError;
    }
    next_frame_++;
    total_frames_checked++;
  }

  return MemoryError::NoPageFramesAvailable;
}

MemoryError PageFrameManager::FreeFrame(uint32 frame_address) {
  SUPPRESS_UNUSED_WARNING(frame_address)
  return MemoryError::NoError;
}

size PageFrameManager::NumFrames() const {
  return num_frames_;
}

FrameTableEntry PageFrameManager::FrameAtIndex(size index) const {
  klib::Assert(index >= 0 && index <= 1024 * 1024);
  return page_frames_[index];
}

size PageFrameManager::NextFrame() const {
  return next_frame_;
}

}  // namespace kernel
