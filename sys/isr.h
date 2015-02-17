#ifndef SYS_ISR_H_
#define SYS_ISR_H_

#include "lib/types.h"

// Registers when the ISR was triggered. Used for (hopefully) diagnosing bugs.
typedef struct {
  // Pushed the segments last.
  uint32_t gs;
  uint32_t fs;
  uint32_t es;
  uint32_t ds;

  // Pushed by "pusha".
  uint32_t edi;
  uint32_t esi;
  uint32_t ebp;
  uint32_t esp;

  uint32_t ebx;
  uint32_t edx;
  uint32_t ecx;
  uint32_t eax;

  // "push byte #" and ecodes does this.
  uint32_t int_no;
  uint32_t err_code;

  // Pushed by the processor automcially.
  uint32_t eip;
  uint32_t cs;
  uint32_t eflags;
  uint32_t useresp;
  uint32_t ss;
} regs;

void isr_install();

#endif  // SYS_ISR_H_
