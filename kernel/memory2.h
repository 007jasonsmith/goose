#ifndef KERNEL_MEMORY2_H_
#define KERNEL_MEMORY2_H_

namespace kernel {

// Initilize the kernel's page directory table with the current binary
// that is loaded (and executing) in memory. This is a required first
// step before bootstrapping the physical memory manager.
void InitializeKernelPageDirectory();

}  // namespace kernel

#endif  // KERNEL_MEMORY2_H_
