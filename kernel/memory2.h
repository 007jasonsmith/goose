#ifndef KERNEL_MEMORY2_H_
#define KERNEL_MEMORY2_H_

namespace kernel {

// Initilize the kernel's page directory table with the current binary
// that is loaded (and executing) in memory. This is a required first
// step before bootstrapping the physical memory manager.
void InitializeKernelPageDirectory();

void InitializePageFrameManager();

// Once the page frame manager and kernel page directory are set up correctly,
// the final step is to synchronize the two. After this point the kernel has
// an accurate accounting of the machine's memory.
void SyncPhysicalAndVirtualMemory();

}  // namespace kernel

#endif  // KERNEL_MEMORY2_H_
