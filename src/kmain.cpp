#include "klib/console.h"
#include "klib/debug.h"
#include "klib/strings.h"
#include "sys/gdt.h"
#include "sys/idt.h"
#include "sys/isr.h"

// EXPERIMENTAL
// Parse memory that is laid out from GRUB.
// http://wiki.osdev.org/Detecting_Memory_%28x86%29#Memory_Map_Via_GRUB
typedef struct aout_symbol_table {
  uint32 tabsize;
  uint32 strsize;
  uint32 addr;
  uint32 reserved;
} aout_symbol_table_t;

typedef struct elf_section_header_table {
  uint32 num;
  uint32 size;
  uint32 addr;
  uint32 shndx;
} elf_section_header_table_t;

typedef struct multiboot_info {
  uint32 flags;
  uint32 mem_lower;
  uint32 mem_upper;
  uint32 boot_device;
  uint32 cmdline;
  uint32 mods_count;
  uint32 mods_addr;
  union {
    aout_symbol_table_t aout_sym;
    elf_section_header_table_t elf_sec;
  } u;
  uint32 mmap_length;
  uint32 mmap_addr;
} multiboot_info_t;

typedef struct multiboot_memory_map {
  uint32 size;
  uint32 base_addr_low,base_addr_high;
  // You can also use: uint32 long int base_addr; if supported.
  uint32 length_low,length_high;
  // You can also use: uint32 long int length; if supported.
  uint32 type;
} multiboot_memory_map_t;

extern "C" {

const char version[] = "v0.1a";
void kmain(multiboot_info_t* mbt, uint32 magic) {
  debug_log("Kernel started.");
  debug_log("sizeof(uint32) = %d", sizeof(uint32));
  debug_log("sizeof(uint32) = %d", sizeof(uint32));

  // Initialize core CPU-based systems.
  gdt_install();  // Global descriptor table.
  idt_install();  // Interrupt descriptor table.
  isr_install();  // Interrupt servicer routines.

  con_initialize();
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

  debug_log("Kernel halted.");
}

}
