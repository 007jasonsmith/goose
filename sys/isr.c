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

  // Pushed by the processor automcially.
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

/* These are function prototypes for all of the exception
*  handlers: The first 32 entries in the IDT are reserved
*  by Intel, and are designed to service exceptions! */
extern void interrupt_handler_0();

/* This is a very repetitive function... it's not hard, it's
*  just annoying. As you can see, we set the first 32 entries
*  in the IDT to the first 32 ISRs. We can't use a for loop
*  for this, because there is no way to get the function names
*  that correspond to that given entry. We set the access
*  flags to 0x8E. This means that the entry is present, is
*  running in ring 0 (kernel level), and has the lower 5 bits
*  set to the required '14', which is represented by 'E' in
*  hex. */
void isr_install()
{
    idt_set_gate(0, (uint32_t) interrupt_handler_0, 0x08, 0x8E);
    // idt_set_gate(1, (unsigned)_isr1, 0x08, 0x8E);
    // idt_set_gate(2, (unsigned)_isr2, 0x08, 0x8E);
    // idt_set_gate(3, (unsigned)_isr3, 0x08, 0x8E);

                             /* Fill in the rest of these ISRs here */

    // idt_set_gate(30, (unsigned)_isr30, 0x08, 0x8E);
    // idt_set_gate(31, (unsigned)_isr31, 0x08, 0x8E);
}

/* This is a simple string array. It contains the message that
*  corresponds to each and every exception. We get the correct
*  message by accessing like:
*  exception_message[interrupt_number] */
const char *exception_messages[] =
{
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    
                 /* Fill in the rest here from our Exceptions table */
    
    "Reserved",
    "Reserved"
};

// TODO(chris): IMPORTANT! Disable interrupts in the assembly code
// before the call to this method, and reenable them afterwards.
// Handling interrupts while handling interrupts is probably a bad idea.

// All interrupts trigger this method.
void interrupt_handler(regs *r) {
  // Processor interrupts.
  // TODO(chris): Handle custom, syscall interrupts.
  if (r->int_no < 32) {
    fb_println("%s", exception_messages[r->int_no]);
    fb_println(" Exception. System Halted!\n");

    kernel_exit();
  }
}
