#include "gtest/gtest.h"

#include "kernel/memory.h"

namespace kernel {

TEST(Memory, Size) {
  PageDirectoryEntry pde;
  EXPECT_EQ(sizeof(pde), 4U);
}

TEST(Memory, PageTableAddress) {
  PageDirectoryEntry pde;
  pde.SetPageTableAddress(0xFFFFFFFFU);

  // Assert that the bottom 10bits are zeroed, since it is 4KiB aligned.
  EXPECT_EQ(pde.GetPageTableAddress(), 0xFFFFF000U);
}

}  // namespace kernel
