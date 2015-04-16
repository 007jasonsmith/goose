#include "gtest/gtest.h"

#include "kernel/memory.h"

namespace kernel {

TEST(PointerTableEntry, Size) {
  PointerTableEntry a;
  EXPECT_EQ(sizeof(a), 4U);

  PageDirectoryEntry pde;
  EXPECT_EQ(sizeof(pde), 4U);

  PageTableEntry pte;
  EXPECT_EQ(sizeof(pte), 4U);

  FrameTableEntry fte;
  EXPECT_EQ(sizeof(fte), 4U);
}

TEST(PointerTableEntry, Address) {
  PointerTableEntry pte;

  // Assert that the bottom 10bits are zeroed, since it is 4KiB aligned.
  pte.SetAddress(0xFFFFFFFFU);
  EXPECT_EQ(pte.Address(), 0xFFFFF000U);

  // Assert it, you know, works.
  pte.SetAddress(0x12345678U);
  EXPECT_EQ(pte.Address(), 0x12345000U);
}

TEST(PointerTableEntry, StatusFlags) {
  PageDirectoryEntry pde;
  EXPECT_EQ(pde.PresentBit(), false);
  pde.SetPresentBit(true);
  EXPECT_EQ(pde.PresentBit(), true);

  pde.SetReadWriteBit(true);
  pde.SetSizeBit(true);
  EXPECT_EQ(pde.Value(), 0b10000011U);
}

TEST(PageFrameManager, Initialize1) {
  MemoryRegion regions[] = {
    {     0, 4096 },  // 1 page frame
    { 12288, 8192 },  // 2 page frames
  };

  PageFrameManager pfm;
  pfm.Initialize(regions, 2);

  EXPECT_EQ(pfm.NumFrames(), 3);

  FrameTableEntry pfe;
  pfe = pfm.FrameAtIndex(0);
  EXPECT_EQ(pfe.Address(), 0x0000U);
  pfe = pfm.FrameAtIndex(1);
  EXPECT_EQ(pfe.Address(), 0x3000U);
  pfe = pfm.FrameAtIndex(2);
  EXPECT_EQ(pfe.Address(), 0x4000U);
}

}  // namespace kernel
