#include "sys/idt.h"

#include "klib/types.h"

// The interrupt descriptor table here registers handlers for all 255 possible
// interrupts, though in practice not every one will be registered. See isr.h/c.
//
// http://en.wikipedia.org/wiki/Interrupt_descriptor_table
// http://wiki.osdev.org/IDT
// http://www.osdever.net/bkerndev/Docs/idt.htm

typedef struct __attribute__((packed)) {
    uint16 base_lo;
    uint16 sel;
    uint8 always0;
    uint8 flags;
    uint16 base_hi;
} IdtEntry;

typedef struct __attribute__((packed)) {
    uint16 limit;
    uint32 base;
} IdtPointer;

// Note that while we reserve *space* for all interrupts, we don't need to
// support them all. If the 'presence' bit is not set, it will trigger an
// "Unhandled Interrupt" exception (which we hopefully will handle).
IdtPointer interrupt_descriptor_table_ptr;
IdtEntry interrupt_descriptor_table[256];

// Defined in idt_asm.s, used to load the IDT.
extern "C" {
extern void idt_load();
}  // extern "C"

namespace sys {

// Setup a descriptor table entry.
void InterruptDescriptorTableSetGate(uint8 index, uint32 base, uint16 sel, uint8 flags) {
  IdtEntry* entry = &interrupt_descriptor_table[index];

  entry->base_lo = (base & 0xFFFF);
  entry->base_hi = (base >> 16) & 0xFFFF;

  entry->sel = sel;
  entry->always0 = 0;
  entry->flags = flags;
}

void InstallInterruptDescriptorTable() {
  interrupt_descriptor_table_ptr.limit = sizeof(interrupt_descriptor_table) - 1;
  interrupt_descriptor_table_ptr.base = (uint32) &interrupt_descriptor_table;

  // Clear out the IDT. Later isr_install will register custom handlers.
  for (size i = 0; i < 256; i++) {
    idt_set_gate(i, 0, 0, 0);
  }
  
  idt_load();
}

}  // namespace sys
