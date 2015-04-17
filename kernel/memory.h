#ifndef KERNEL_MEMORY_H_
#define KERNEL_MEMORY_H_

#include "klib/types.h"
#include "kernel/boot.h"

namespace kernel {

// Basic class that keeps a 4KiB aligned pointer, and provides 12 status bits
// for other flags. Surprisingly useful.
class PointerTableEntry {
 public:
  explicit PointerTableEntry();

  uint32 Address() const;
  void SetAddress(uint32 address);

  uint32 Value() const;

 protected:
  bool StatusFlag(size bit) const;
  void SetStatusFlag(size bit, bool value);

  uint32 data_;
};


// Macro for reducing boiler plate.
#define BIT_FLAG_PROPS(name)     \
  bool name##Bit() const;        \
  void Set##name##Bit(bool bit);

// 32-bit / 4KiB / no-PSE page directory table entry.
class PageDirectoryEntry : public PointerTableEntry {
 public:
  explicit PageDirectoryEntry();

  // Bits
  // 31 - 11: 4KiB aligned pointer to a PageTableEntry.
  // 7: (S) Size. If set, pages are 4MiB in size. Otherwise 4KiB. 4MiB also
  //        need PSE to be enabled.
  // 5: (A) Accessed. Hold if the page has been read/written to.
  // 4: (D) If set the page will not be cached. If not, could be.
  // 3: (W) Write-Through. If set write-through cache is enabled. Else
  //        write-back is enabled.
  // 2: (U) User.If set, page can be read by user space. If NOT set, can
  //        only be read by privledged code.
  // 1: (R) Read/Write. If set, page is read/write, otherwise read-only.
  // 0: (P) Present. Does the page exist in pysical memory?
  BIT_FLAG_PROPS(Present)
  BIT_FLAG_PROPS(ReadWrite)
  BIT_FLAG_PROPS(User)
  BIT_FLAG_PROPS(WriteThrough)
  BIT_FLAG_PROPS(DisableCache)
  BIT_FLAG_PROPS(Accessed)
  BIT_FLAG_PROPS(Size)
};

// 32-bit / 4KiB / no-PAE page table entry.
class PageTableEntry : public PointerTableEntry {
 public:
  explicit PageTableEntry();

  // Bits
  // 31 - 11: 4KiB aligned pointer to a PageTableEntry.
  // 7: (S) Size. If set, pages are 4MiB in size. Otherwise 4KiB. 4MiB also
  //        need PSE to be enabled.
  // 5: (A) Accessed. Hold if the page has been read/written to.
  // 4: (D) If set the page will not be cached. If not, could be.
  // 3: (W) Write-Through. If set write-through cache is enabled. Else
  //        write-back is enabled.
  // 2: (U) User.If set, page can be read by user space. If NOT set, can
  //        only be read by privledged code.
  // 1: (R) Read/Write. If set, page is read/write, otherwise read-only.
  // 0: (P) Present. Does the page exist in pysical memory?
  BIT_FLAG_PROPS(Present)
  BIT_FLAG_PROPS(ReadWrite)
  BIT_FLAG_PROPS(User)
  BIT_FLAG_PROPS(WriteThrough)
  BIT_FLAG_PROPS(DisableCache)
  BIT_FLAG_PROPS(Accessed)
  BIT_FLAG_PROPS(Dirty)
  BIT_FLAG_PROPS(Global)
};

// Description of a physical frame.
class FrameTableEntry : public PointerTableEntry {
public:
  explicit FrameTableEntry();

  // Bits
  // 31 - 11: 4KiB aligned pointer to a page frame.
  // 0: (U) In-use. Is the frame currently in-use.
  BIT_FLAG_PROPS(InUse)
};
#undef BIT_FLAG_PROPS

enum class MemoryError {
  // No error associated with the operation.
  NoError = 0,

  // The provided page frame address was invalid.
  InvalidPageFrameAddress = 1,  

  // The request for a page frame failed because no more are available.
  NoPageFramesAvailable = 2,

  // The specified address was invalid because it was not properly aligned.
  UnalignedAddress = 3,

  // Frame was double-freed.
  PageFrameAlreadyFree = 4
};

struct MemoryRegion {
  uint32 address;
  uint32 size;
};

// Physical memory manager. Keeping track of all available physical frames
// and their state.
// TODO(chris): Switch to using a more efficent mechanism, e.g.
// http://en.wikipedia.org/wiki/Buddy_memory_allocation
// As-is this takes up 4MiB of RAM regardless of available system memory.
class PageFrameManager {
 public:
  explicit PageFrameManager();

  void Initialize(const MemoryRegion* regions, size region_count);

 public:
  // Returns the next free page frame, marking it as in-use.
  MemoryError RequestFrame(uint32* out_address);
  // Free the given memory frame.
  MemoryError FreeFrame(uint32 frame_address);

  size NumFrames() const;
  FrameTableEntry FrameAtIndex(size index) const;

  // TODO(chris): Make this private, only for tests.
  size NextFrame() const;

 private:
  // Index of the next free frame. Loops around once each frame has been
  // taken.
  size next_frame_;

  // Total number of page frames available.
  size num_frames_;

  // We provide a page frame entry for every possible page frame, spamming the
  // full 4GiB of addressable memory. It is unlikely that this will all be
  // usable, however it is statically allocated here for convience.
  FrameTableEntry page_frames_[1024 * 1024];
};

}  // namespace kernel

#endif  // KERNEL_MEMORY_H_
