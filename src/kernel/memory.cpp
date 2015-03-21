#include "kernel/memory.h"

namespace {

// System boot info.
const kernel::grub::multiboot_info_t* boot_info_ = nullptr;

}  // anonymous namespace

namespace kernel {

void SetMultibootInfo(const grub::multiboot_info_t* boot_info) {
  // CHECK_NOTNULL, CHECK_NULL, etc.
  boot_info_ = boot_info;
}

const grub::multiboot_info_t*  GetMultibootInfo() {
  return boot_info_;
}

}  // namespace kernel
