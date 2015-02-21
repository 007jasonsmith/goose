#include "sys/gdt.h"

#include "klib/types.h"

// The global descriptor table here is very basic. We only install three
// entries, a null segment, and others for code and data. Each overlapping
// and spanning the full 4GiB of addressable memory.
//
// http://en.wikipedia.org/wiki/Global_Descriptor_Table
// http://wiki.osdev.org/Global_Descriptor_Table
// http://www.osdever.net/bkerndev/Docs/gdt.htm

typedef struct __attribute__((packed)) {
  uint16 limit_low;
  uint16 base_low;
  uint8  base_middle;
  uint8  access;
  uint8  granularity;
  uint8  base_high;
} GdtEntry;

typedef struct __attribute__((packed)) {
  uint16 limit;
  uint32 base;
} GdtPointer;

// Our GDT only has three entries. Initialized via gdt_install.
GdtPointer global_descriptor_table_ptr;
GdtEntry global_descriptor_table[3];

// Defined in gdt_asm.s, used to properly clear out the existing GDT (from the
// boot loader) and replace it with our own.
extern void gdt_flush();

// Setup a descriptor table entry.
void gdt_set_gate(GdtEntry* entry,
		  uint32 base,
		  uint32 limit,
		  uint8 access,
		  uint8 gran) {
  entry->base_low = (base & 0xFFFF);
  entry->base_middle = (base >> 16) & 0xFF;
  entry->base_high = (base >> 24) & 0xFF;

  entry->limit_low = (limit & 0xFFFF);
  entry->granularity = ((limit >> 16) & 0x0F);

  entry->granularity |= (gran & 0xF0);
  entry->access = access;
}

void gdt_install() {
  global_descriptor_table_ptr.limit = sizeof(global_descriptor_table) - 1;
  global_descriptor_table_ptr.base = (uint32) &global_descriptor_table;

  // null, code, and data segments. The base address is 0, and the limit is
  // 4GiB. Uses 32-bit opcodes.
  gdt_set_gate(&global_descriptor_table[0],
	       0, 0, 0, 0);
  gdt_set_gate(&global_descriptor_table[1],
	       0, 0xFFFFFFFF, 0x9A, 0xCF);
  gdt_set_gate(&global_descriptor_table[2],
	       0, 0xFFFFFFFF, 0x92, 0xCF);

  gdt_flush();
}
