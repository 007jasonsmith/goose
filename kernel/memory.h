// Memory map.

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

// Initilize the kernel's page directory table with the current binary
// that is loaded (and executing) in memory. This is a required first
// step before bootstrapping the physical memory manager.
void InitializeKernelPageDirectory();

}  // namespace kernel

#endif  // KERNEL_MEMORY_H_
