#include "gtest/gtest.h"

#include "kernel/memory.h"

namespace kernel {

TEST(PageDirectoryEntry, Size) {
  PageDirectoryEntry pde;
  EXPECT_EQ(sizeof(pde), 4U);
}

TEST(PageDirectoryEntry, PageTableAddress) {
  PageDirectoryEntry pde;

  // Assert that the bottom 10bits are zeroed, since it is 4KiB aligned.
  pde.SetPageTableAddress(0xFFFFFFFFU);
  EXPECT_EQ(pde.GetPageTableAddress(), 0xFFFFF000U);

  // Assert it, you know, works.
  pde.SetPageTableAddress(0x12345678U);
  EXPECT_EQ(pde.GetPageTableAddress(), 0x12345000U);
}

TEST(PageDirectoryEntry, Bits) {
  PageDirectoryEntry pde;
  EXPECT_EQ(pde.GetPresentBit(), false);
  pde.SetPresentBit(true);
  EXPECT_EQ(pde.GetPresentBit(), true);

  // TODO(chris): Boiler plate tests for other bit flags.
}

}  // namespace kernel
