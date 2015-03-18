#include "klib/debug.h"
#include "klib/strings.h"
#include "sys/gdt.h"
#include "sys/idt.h"
#include "sys/isr.h"
#include "klib/macros.h"
#include "hal/serial_port.h"
#include "hal/text_ui.h"
#include "kernel/memory.h"

using klib::Debug;

extern "C" {

const char version[] = "v0.1a";
void kmain(kernel::grub::multiboot_info_t* mbt) {
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

  hal::TextUI::SetChar(0, 0, 'G');
  hal::TextUI::SetChar(1, 0, 'o');
  hal::TextUI::SetChar(2, 0, 'o');
  hal::TextUI::SetChar(3, 0, 's');
  hal::TextUI::SetChar(4, 0, 'e');

  #if false
  con_write(HEADER_WIN, "Goose %s", version);

  // EXPERIMENTAL
  con_writeline(OUTPUT_WIN, "memory map = %p, magic = %d", mbt, magic);
  con_writeline(OUTPUT_WIN, "mem %u %u KiB / %u %u MiB", mbt->mem_lower, mbt->mem_upper, mbt->mem_lower / 1024, mbt->mem_upper / 1024);

  multiboot_memory_map_t* mmap = (multiboot_memory_map*) mbt->mmap_addr;
  int map_idx = 0;
  while (((uint32) mmap) < mbt->mmap_addr + mbt->mmap_length) {
    con_writeline(OUTPUT_WIN, "Memory Map [%d][%p] : size %d, type %d", map_idx, mmap, mmap->size, mmap->type);
    con_writeline(OUTPUT_WIN, "  address %d / %d", mmap->base_addr_low, mmap->base_addr_high);
    con_writeline(OUTPUT_WIN, "  length  %d / %d", mmap->length_low, mmap->length_high);

    mmap = (multiboot_memory_map_t*) ( (uint32) mmap + mmap->size + sizeof(uint32) );
    map_idx++;
  }

  con_writeline(DEBUG_WIN, "G[%c]ose", 'o');

  con_writeline(OUTPUT_WIN, "Keyboard echo.");

  char command[256];
  while (true) {
    con_write(OUTPUT_WIN, "> ");
    con_win_readline(OUTPUT_WIN, command, 256);
    con_writeline(OUTPUT_WIN, "Executing '%s'", command);

    if (str_compare(command, "crash")) {
      int denum = 1;
      int result = 0 / (denum - 1);
      con_writeline(OUTPUT_WIN, "Result was %d", result);
    }
    if (str_compare(command, "exit")) {
      break;
    }
  }
  #endif

  Debug::Log("Kernel halted.");
}

}  // extern "C"
