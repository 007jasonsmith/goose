#ifndef SYS_CONTROL_REGISTERS_H_
#define SYS_CONTORL_REGISTERS_H_

#include "klib/types.h"

extern "C" {

uint32 get_cr2();
uint32 get_cr3();

// This is CR3, rename or something?
/**
 * load_pdt:
 *  Loages a page directory table.
 *
 *  @param addr The address of the new page directory table.
 */
void load_pdt(uint32 address);

}  // extern "C"

#endif  // SYS_CONTROL_REGISTERS_H_
