#ifndef SYS_IDT_H_
#define SYS_IDT_H_

#include "lib/types.h"

// Install the system's interrupt descriptor table.
void idt_install();

// Update the IDT to register a new interrupt service routine.
void idt_set_gate(uint8_t index, uint32_t base, uint16_t sel, uint8_t flags);

#endif  // SYS_IDT_H_
