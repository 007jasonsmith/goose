#include "sys/isr.h"

#include "lib/framebuffer.h"
#include "sys/idt.h"
#include "sys/kernel.h"

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

  // Pushed by the processor automatically.
  uint32_t eip;
  uint32_t cs;
  uint32_t eflags;
  uint32_t useresp;
  uint32_t ss;
} regs;


// The actual interrupt service routines that process traps and events.
//
// http://en.wikipedia.org/wiki/Interrupt_handler
// http://wiki.osdev.org/Interrupt_Service_Routines
// http://www.osdever.net/bkerndev/Docs/isrs.htm

// The first 32 interrupts are reserved for hardware exceptions. See
// kExceptionMessages below for descriptions.
extern void interrupt_handler_0();
extern void interrupt_handler_1();
extern void interrupt_handler_2();
extern void interrupt_handler_3();
extern void interrupt_handler_4();
extern void interrupt_handler_5();
extern void interrupt_handler_6();
extern void interrupt_handler_7();
extern void interrupt_handler_8();
extern void interrupt_handler_9();
extern void interrupt_handler_10();
extern void interrupt_handler_11();
extern void interrupt_handler_12();
extern void interrupt_handler_13();
extern void interrupt_handler_14();
extern void interrupt_handler_15();
extern void interrupt_handler_16();
extern void interrupt_handler_17();
extern void interrupt_handler_18();
extern void interrupt_handler_19();
extern void interrupt_handler_20();
extern void interrupt_handler_21();
extern void interrupt_handler_22();
extern void interrupt_handler_23();
extern void interrupt_handler_24();
extern void interrupt_handler_25();
extern void interrupt_handler_26();
extern void interrupt_handler_27();
extern void interrupt_handler_28();
extern void interrupt_handler_29();
extern void interrupt_handler_30();
extern void interrupt_handler_31();

// User friendly descriptions of CPU-defined interrupts.
// "Intel® 64 and IA-32 Architectures Software Developer’s Manual
// Volume 3A: System Programming Guide, Part 1"
const char *kInterruptDescriptions[] = {
    "Division By Zero",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved (15)",
    "x87 FPU Floating-Point Error",
    "Alignment Check Exception",
    "Machine Check Exception",
    "SIMD Floating-Point Exception",
    "Reserved (20)",
    "Reserved (21)",
    "Reserved (22)",
    "Reserved (23)",
    "Reserved (24)",
    "Reserved (25)",
    "Reserved (26)",
    "Reserved (27)",
    "Reserved (28)",
    "Reserved (29)",
    "Reserved (30)",
    "Reserved (31)",
};

void isr_install() {
  // Register all known ISRs. The first 32 are CPU-defined.
  idt_set_gate(0, (uint32_t) interrupt_handler_0, 0x08, 0x8E);
  idt_set_gate(1, (uint32_t) interrupt_handler_1, 0x08, 0x8E);
  idt_set_gate(2, (uint32_t) interrupt_handler_2, 0x08, 0x8E);
  idt_set_gate(3, (uint32_t) interrupt_handler_3, 0x08, 0x8E);
  idt_set_gate(4, (uint32_t) interrupt_handler_4, 0x08, 0x8E);
  idt_set_gate(5, (uint32_t) interrupt_handler_5, 0x08, 0x8E);
  idt_set_gate(6, (uint32_t) interrupt_handler_6, 0x08, 0x8E);
  idt_set_gate(7, (uint32_t) interrupt_handler_7, 0x08, 0x8E);
  idt_set_gate(8, (uint32_t) interrupt_handler_8, 0x08, 0x8E);
  idt_set_gate(9, (uint32_t) interrupt_handler_9, 0x08, 0x8E);
  idt_set_gate(10, (uint32_t) interrupt_handler_10, 0x08, 0x8E);
  idt_set_gate(11, (uint32_t) interrupt_handler_11, 0x08, 0x8E);
  idt_set_gate(12, (uint32_t) interrupt_handler_12, 0x08, 0x8E);
  idt_set_gate(13, (uint32_t) interrupt_handler_13, 0x08, 0x8E);
  idt_set_gate(14, (uint32_t) interrupt_handler_14, 0x08, 0x8E);
  idt_set_gate(15, (uint32_t) interrupt_handler_15, 0x08, 0x8E);
  idt_set_gate(16, (uint32_t) interrupt_handler_16, 0x08, 0x8E);
  idt_set_gate(17, (uint32_t) interrupt_handler_17, 0x08, 0x8E);
  idt_set_gate(18, (uint32_t) interrupt_handler_18, 0x08, 0x8E);
  idt_set_gate(19, (uint32_t) interrupt_handler_19, 0x08, 0x8E);
  idt_set_gate(20, (uint32_t) interrupt_handler_20, 0x08, 0x8E);
  idt_set_gate(21, (uint32_t) interrupt_handler_21, 0x08, 0x8E);
  idt_set_gate(22, (uint32_t) interrupt_handler_22, 0x08, 0x8E);
  idt_set_gate(23, (uint32_t) interrupt_handler_23, 0x08, 0x8E);
  idt_set_gate(24, (uint32_t) interrupt_handler_24, 0x08, 0x8E);
  idt_set_gate(25, (uint32_t) interrupt_handler_25, 0x08, 0x8E);
  idt_set_gate(26, (uint32_t) interrupt_handler_26, 0x08, 0x8E);
  idt_set_gate(27, (uint32_t) interrupt_handler_27, 0x08, 0x8E);
  idt_set_gate(28, (uint32_t) interrupt_handler_28, 0x08, 0x8E);
  idt_set_gate(29, (uint32_t) interrupt_handler_29, 0x08, 0x8E);
  idt_set_gate(30, (uint32_t) interrupt_handler_30, 0x08, 0x8E);
  idt_set_gate(31, (uint32_t) interrupt_handler_31, 0x08, 0x8E);
}

// All interrupts trigger this method. Note that all interrupts are disabled
// for the duration of this method. So when we start doing "real work" in
// these ISRs, save the state somewhere, and return immediately. Doing the
// long-running work async, and letting other interrupts file as normal.
void interrupt_handler(regs *r) {
  // Processor interrupts.
  const char* description = "Unknown Interrupt";
  if (r->int_no < 32) {
    description = kInterruptDescriptions[r->int_no];
  }

  // TODO(chris): Make a kick-ass BSOD.
  fb_println("Received interrupt %s[%d] with code %d",
	     description, r->int_no, r->err_code);
  fb_println("System Halted\n");
  kernel_exit();
}
