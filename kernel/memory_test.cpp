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

TEST(PageFrameManager, Initialize) {
  MemoryRegion regions[] = {
    {     0, 4096 },  // 1 page frame
    { 12288, 8192 },  // 2 page frames
  };

  PageFrameManager pfm;
  pfm.Initialize(regions, 2);

  EXPECT_EQ(pfm.NumFrames(), 3);

  FrameTableEntry fte;
  fte = pfm.FrameAtIndex(0);
  EXPECT_EQ(fte.Address(), 0x0000U);
  fte = pfm.FrameAtIndex(1);
  EXPECT_EQ(fte.Address(), 0x3000U);
  fte = pfm.FrameAtIndex(2);
  EXPECT_EQ(fte.Address(), 0x4000U);
}

TEST(PageFrameManager, Initialize_MultiFrames) {
  MemoryRegion regions[] = {
    { 1024 * 1024, 8 * 1024 * 1024 },  // 8MiB starting at 1MiB.
  };

  PageFrameManager pfm;
  pfm.Initialize(regions, 1);

  EXPECT_EQ(pfm.NumFrames(), 2 * 1024);

  FrameTableEntry fte;
  fte = pfm.FrameAtIndex(0);
  EXPECT_EQ(fte.Address(), 0x100000U);
}


TEST(PageFrameManager, Initialize_Alignment) {
  MemoryRegion regions[] = {
    {     1, 4096 },  // Address non, aligned. 0 frames.
    { 8192,  4095 },  // Size < 4096. 0 frames.
    { 5 * 4096 + 1, 4096 * 2 - 2 },  // Almost a full frame. Almost.
    { 10 * 4096 + 1, 8192 }  // One frame, starting at 11*4096.
  };

  PageFrameManager pfm;
  pfm.Initialize(regions, 4);

  EXPECT_EQ(pfm.NumFrames(), 1);

  FrameTableEntry fte;
  fte = pfm.FrameAtIndex(0);
  EXPECT_EQ(fte.Address(), 11 * 4096U);
}

TEST(PageFrameManager, RequestFrame) {
  MemoryRegion regions[] = {
    { 0, 8192 }
  };

  PageFrameManager pfm;
  pfm.Initialize(regions, 1);
  EXPECT_EQ(pfm.NumFrames(), 2);

  uint32 address;
  EXPECT_EQ(MemoryError::NoError, pfm.RequestFrame(&address));
  EXPECT_EQ(address, 0x0U);

  EXPECT_EQ(MemoryError::NoError, pfm.RequestFrame(&address));
  EXPECT_EQ(address, 0x1000U);

  EXPECT_EQ(MemoryError::NoPageFramesAvailable, pfm.RequestFrame(&address));
}

TEST(PageFrameManager, ReserveFrame) {
  MemoryRegion regions[] = {
    { 0, 8192 }
  };

  PageFrameManager pfm;
  pfm.Initialize(regions, 1);
  EXPECT_EQ(pfm.NumFrames(), 2);

  EXPECT_EQ(MemoryError::NoError, pfm.ReserveFrame(0x0U));
  EXPECT_EQ(MemoryError::NoError, pfm.ReserveFrame(0x1000U));

  uint32 address;
  EXPECT_EQ(MemoryError::NoPageFramesAvailable, pfm.RequestFrame(&address));
}

TEST(PageFrameManager, ReserveFrame_Errors) {
  MemoryRegion regions[] = {
    { 0, 8192 }
  };

  PageFrameManager pfm;
  pfm.Initialize(regions, 1);
  EXPECT_EQ(pfm.NumFrames(), 2);

  EXPECT_EQ(MemoryError::UnalignedAddress, pfm.ReserveFrame(4000U));

  uint32 address;
  EXPECT_EQ(MemoryError::NoError, pfm.RequestFrame(&address));
  EXPECT_EQ(0x0U, address);
  EXPECT_EQ(MemoryError::PageFrameAlreadyInUse, pfm.ReserveFrame(0x0U));
  EXPECT_EQ(MemoryError::InvalidPageFrameAddress, pfm.ReserveFrame(4096 * 4U));
}

TEST(PageFrameManager, FreeFrame) {
  MemoryRegion regions[] = {
    { 0, 8192 }
  };

  PageFrameManager pfm;
  pfm.Initialize(regions, 1);
  EXPECT_EQ(pfm.NumFrames(), 2);

  uint32 address;
  // Request/reserve both frames, then get error NoFramesAvailable.
  EXPECT_EQ(MemoryError::NoError, pfm.ReserveFrame(0x0U));
  EXPECT_EQ(MemoryError::NoError, pfm.RequestFrame(&address));
  EXPECT_EQ(address, 0x1000U);
  EXPECT_EQ(MemoryError::NoPageFramesAvailable, pfm.RequestFrame(&address));

  // Free a frame, verify it can be retrieved.
  EXPECT_EQ(MemoryError::NoError, pfm.FreeFrame(0x1000U));
  EXPECT_EQ(MemoryError::NoError, pfm.RequestFrame(&address));
  EXPECT_EQ(address, 0x1000U);

  EXPECT_EQ(MemoryError::NoPageFramesAvailable, pfm.RequestFrame(&address));
}

TEST(PageFrameManager, FreeFrame_Errors) {
  MemoryRegion regions[] = {
    { 0, 8192 }
  };

  PageFrameManager pfm;
  pfm.Initialize(regions, 1);
  EXPECT_EQ(pfm.NumFrames(), 2);

  EXPECT_EQ(MemoryError::UnalignedAddress, pfm.FreeFrame(4000U));
  EXPECT_EQ(MemoryError::PageFrameAlreadyFree, pfm.FreeFrame(0x0U));
  EXPECT_EQ(MemoryError::InvalidPageFrameAddress, pfm.FreeFrame(4096 * 4U));
}

}  // namespace kernel
