#include "kernel/boot.h"

namespace {

// System boot info.
const kernel::grub::multiboot_info* boot_info_ = nullptr;

}  // anonymous namespace

namespace kernel {

void SetMultibootInfo(const grub::multiboot_info* boot_info) {
  // CHECK_NOTNULL, CHECK_NULL, etc.
  boot_info_ = boot_info;
}

const grub::multiboot_info*  GetMultibootInfo() {
  return boot_info_;
}

}  // namespace kernel
