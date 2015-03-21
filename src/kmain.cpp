#include "klib/debug.h"
#include "klib/strings.h"
#include "sys/gdt.h"
#include "sys/idt.h"
#include "sys/isr.h"
#include "klib/macros.h"
#include "hal/serial_port.h"
#include "hal/text_ui.h"
#include "kernel/memory.h"
#include "shell/shell.h"

using klib::Debug;

extern "C" {

void __cxa_pure_virtual() {
  // TODO(chris): Panic
  Debug::Log("__cxa_pure_virtual()");
}

const char version[] = "v0.1a";
void kmain(const kernel::grub::multiboot_info_t* mbt) {
  SUPPRESS_UNUSED_WARNING(mbt);

  // Initialize device drivers.
  hal::SerialPort::Initialize();
  hal::TextUI::Initialize();

  // Register the debug log. Debug messages are written to COM1, which
  // the CPU emulator will kindly pipe to a file.
  hal::SerialPortOutputFn serial_port_writer;
  Debug::RegisterOutputFn(&serial_port_writer);

  Debug::Log("Kernel started.");

  // Initialize core CPU-based systems.
  sys::InstallGlobalDescriptorTable();
  sys::InstallInterruptDescriptorTable();
  sys::InstallInterruptServiceRoutines();

  kernel::SetMultibootInfo(mbt);

  shell::Run();

  Debug::Log("Kernel halted.");
}

}  // extern "C"
