#include "sys/idt.h"

#include "lib/types.h"

// The interrupt descriptor table here registers handlers for all 255 possible
// interrupts, though in practice not every one will be registered. See isr.h/c.

typedef struct __attribute__((packed)) {
    uint16_t base_lo;
    uint16_t sel;
    uint8_t always0;
    uint8_t flags;
    uint16_t base_hi;
} IdtEntry;

typedef struct __attribute__((packed)) {
    uint16_t limit;
    uint32_t base;
} IdtPointer;

// Note that while we reserve *space* for all interrupts, we don't need to
// support them all. If the 'presence' bit is not set, it will trigger an
// "Unhandled Interrupt" exception (which we hopefully will handle).
IdtPointer interrupt_descriptor_table_ptr;
IdtEntry interrupt_descriptor_table[256];

// Defined in idt_asm.s, used to load the IDT.
extern void idt_load();

// Setup a descriptor table entry.
void idt_set_gate(uint8_t index, uint32_t base, uint16_t sel, uint8_t flags) {
  IdtEntry* entry = &interrupt_descriptor_table[index];

  entry->base_lo = (base & 0xFFFF);
  entry->base_hi = (base >> 16) & 0xFFFF;

  entry->sel = sel;
  entry->always0 = 0;
  entry->flags = flags;
}

void idt_install() {
  interrupt_descriptor_table_ptr.limit = sizeof(interrupt_descriptor_table) - 1;
  interrupt_descriptor_table_ptr.base = (uint32_t) &interrupt_descriptor_table;

  // Clear out the IDT. Later isr_install will register custom handlers.
  for (size_t i = 0; i < 256; i++) {
    idt_set_gate(i, 0, 0, 0);
  }
  
  idt_load();
}
