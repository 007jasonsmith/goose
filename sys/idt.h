#ifndef SYS_IDT_H_
#define SYS_IDT_H_

#include "klib/types.h"

// Install the system's interrupt descriptor table.
void idt_install();

// Update the IDT to register a new interrupt service routine.
void idt_set_gate(uint8 index, uint32 base, uint16 sel, uint8 flags);

#endif  // SYS_IDT_H_
