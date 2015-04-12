#ifndef SYS_IDT_H_
#define SYS_IDT_H_

#include "klib/types.h"

namespace sys {

// Install the system's interrupt descriptor table.
void InstallInterruptDescriptorTable();

// Update the IDT to register a new interrupt service routine.
// i.e. register a new interrupt.
void InterruptDescriptorTableSetGate(
    uint8 index, uint32 base, uint16 sel, uint8 flags);

}  // namespace sys

#endif  // SYS_IDT_H_
