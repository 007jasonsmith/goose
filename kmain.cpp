#include "klib/debug.h"
#include "klib/panic.h"
#include "klib/strings.h"
#include "sys/gdt.h"
#include "sys/halt.h"
#include "sys/idt.h"
#include "sys/isr.h"
#include "klib/macros.h"
#include "hal/serial_port.h"
#include "hal/text_ui.h"
#include "kernel/boot.h"
#include "shell/shell.h"

using klib::Debug;
using hal::Color;
using hal::TextUI;

extern "C" {

const uint32 kMultibootMagicNumber = 0x2BADB002;

// Handler for a C++ pure-virtual call. Panics.
void __cxa_pure_virtual();

// Kernel panic function handler.
void PanicHandler(const char* message); 

// Friendly system shutdown screen.
void FriendlyShutdown(const char* message);

const char version[] = "v0.1a";
void kmain(uint32 multiboot_magic, const kernel::grub::multiboot_info* mbt) {
  // Initialize device drivers.
  hal::SerialPort::Initialize();
  hal::TextUI::Initialize();

  // Register the debug log. Debug messages are written to COM1, which
  // the CPU emulator will kindly pipe to a file.
  hal::SerialPortOutputFn serial_port_writer;
  Debug::RegisterOutputFn(&serial_port_writer);

  Debug::Log("Kernel started.");

  klib::SetPanicFn(&PanicHandler);
  if (multiboot_magic != kMultibootMagicNumber) {
    klib::Panic("Not booted by GRUB. Not sure what happened.");
  }

  // Initialize core CPU-based systems.
  sys::InstallGlobalDescriptorTable();
  sys::InstallInterruptDescriptorTable();
  sys::InstallInterruptServiceRoutines();

  kernel::SetMultibootInfo(mbt);
  shell::Run();

  Debug::Log("Kernel halted.");
  FriendlyShutdown("Kernel Halted");
}

void __cxa_pure_virtual() {
  klib::Panic("Pure virtual function call.");
}

void PanicHandler(const char* message) {
  Debug::Log("************");
  Debug::Log("KERNEL PANIC");
  Debug::Log("************");
  Debug::Log(message);

  // Print a RSOD
  for (int y = 0; y < 25; y++) {
    for (int x = 0; x < 80; x++) {
      TextUI::SetColor(x, y, Color::White, Color::Red);
      TextUI::SetChar(x, y, ' ');
    }
  }
  int pos = 40 - klib::length(message) / 2;
  if (pos < 0) {
    pos = 0;
  }
  TextUI::Print(message, pos, 12);
  TextUI::ShowCursor(false);

  system_halt();
}

void FriendlyShutdown(const char* message) {
  Debug::Log("*****************");
  Debug::Log("FRIENDLY SHUTDOWN");
  Debug::Log("*****************");
  Debug::Log(message);

  // Print a BSO.. fun?
  for (int y = 0; y < 25; y++) {
    for (int x = 0; x < 80; x++) {
      TextUI::SetColor(x, y, Color::White, Color::Blue);
      TextUI::SetChar(x, y, ' ');
    }
  }
  int pos = 40 - klib::length(message) / 2;
  if (pos < 0) {
    pos = 0;
  }
  TextUI::Print(message, pos, 12);
  TextUI::ShowCursor(false);

  system_halt();
}

}  // extern "C"
