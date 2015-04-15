#include "sys/control_registers.h"

#include "klib/types.h"

extern "C" {

uint32 get_cr2();

uint32 get_cr3();
void set_cr3(uint32 address);

uint32 get_cr4();
void set_cr4(uint32 value);
