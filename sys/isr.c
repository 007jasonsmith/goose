#include "sys/isr.h"

#include "lib/framebuffer.h"
#include "sys/idt.h"

/* These are function prototypes for all of the exception
*  handlers: The first 32 entries in the IDT are reserved
*  by Intel, and are designed to service exceptions! */
extern void _isr0();
extern void _isr1();
extern void _isr2();
extern void _isr3();

                         /* Fill in the rest of the ISR prototypes here */

extern void _isr29();
extern void _isr30();
extern void _isr31();

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
    idt_set_gate(0, (unsigned)_isr0, 0x08, 0x8E);
    idt_set_gate(1, (unsigned)_isr1, 0x08, 0x8E);
    idt_set_gate(2, (unsigned)_isr2, 0x08, 0x8E);
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

/* All of our Exception handling Interrupt Service Routines will
*  point to this function. This will tell us what exception has
*  happened! Right now, we simply halt the system by hitting an
*  endless loop. All ISRs disable interrupts while they are being
*  serviced as a 'locking' mechanism to prevent an IRQ from
*  happening and messing up kernel data structures */
void fault_handler(regs *r) {
    /* Is this a fault whose number is from 0 to 31? */
    if (r->int_no < 32)
    {
        /* Display the description for the Exception that occurred.
        *  In this tutorial, we will simply halt the system using an
        *  infinite loop */
        fb_println("%s", exception_messages[r->int_no]);
        fb_println(" Exception. System Halted!\n");
	// TOOD(chris): Call an assembly method.
	// To actually halt we need to disable interrupts
	// and halt the CPU.
        for (;;);
    }
}
