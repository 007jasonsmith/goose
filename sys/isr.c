#include "sys/isr.h"

#include "lib/framebuffer.h"
#include "sys/io.h"
#include "sys/idt.h"
#include "sys/kernel.h"
#include "sys/keyboard_us.h"

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
// We use the next 16 IDT slots for handling IRQs.
extern void irq_handler_32();
extern void irq_handler_33();
extern void irq_handler_34();
extern void irq_handler_35();
extern void irq_handler_36();
extern void irq_handler_37();
extern void irq_handler_38();
extern void irq_handler_39();
extern void irq_handler_40();
extern void irq_handler_41();
extern void irq_handler_42();
extern void irq_handler_43();
extern void irq_handler_44();
extern void irq_handler_45();
extern void irq_handler_46();
extern void irq_handler_47();

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
  // By default IRQs are mapped to IDT entries 8-15, but in protected mode
  // those indexes take on a different meaning. Remap IRQ handlers.
  outb(0x20, 0x11);
  outb(0xA0, 0x11);
  outb(0x21, 0x20);
  outb(0xA1, 0x28);
  outb(0x21, 0x04);
  outb(0xA1, 0x02);
  outb(0x21, 0x01);
  outb(0xA1, 0x01);
  outb(0x21, 0x0);
  outb(0xA1, 0x0);

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
  // The next 16 are used for handling IRQs
  idt_set_gate(32, (uint32_t) irq_handler_32, 0x08, 0x8E);
  idt_set_gate(33, (uint32_t) irq_handler_33, 0x08, 0x8E);
  idt_set_gate(34, (uint32_t) irq_handler_34, 0x08, 0x8E);
  idt_set_gate(35, (uint32_t) irq_handler_35, 0x08, 0x8E);
  idt_set_gate(36, (uint32_t) irq_handler_36, 0x08, 0x8E);
  idt_set_gate(37, (uint32_t) irq_handler_37, 0x08, 0x8E);
  idt_set_gate(38, (uint32_t) irq_handler_38, 0x08, 0x8E);
  idt_set_gate(39, (uint32_t) irq_handler_39, 0x08, 0x8E);
  idt_set_gate(30, (uint32_t) irq_handler_40, 0x08, 0x8E);
  idt_set_gate(41, (uint32_t) irq_handler_41, 0x08, 0x8E);
  idt_set_gate(42, (uint32_t) irq_handler_42, 0x08, 0x8E);
  idt_set_gate(43, (uint32_t) irq_handler_43, 0x08, 0x8E);
  idt_set_gate(44, (uint32_t) irq_handler_44, 0x08, 0x8E);
  idt_set_gate(45, (uint32_t) irq_handler_45, 0x08, 0x8E);
  idt_set_gate(46, (uint32_t) irq_handler_46, 0x08, 0x8E);
  idt_set_gate(47, (uint32_t) irq_handler_47, 0x08, 0x8E);

  // Set up timer cycle.
  // TODO(chris): Research modes, etc.
  // TODO(chris): Create some global variables...
  int hz = 1;
  int divisor = 1193180 / hz;
  outb(0x43, 0x36);
  outb(0x40, divisor & 0xFF);
  outb(0x40, divisor >> 8);
 
  // TODO(chris): Wrap this in a file "asm_ops.h" or similer.
  // Safe to handle interrupts.
  __asm__ __volatile__ ("sti");
}

// All interrupts trigger this method. Note that all interrupts are disabled
// for the duration of this method. So when we start doing "real work" in
// these ISRs, save the state somewhere, and return immediately. Doing the
// long-running work async, and letting other interrupts file as normal.
void interrupt_handler(regs* r) {
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

// TODO(chris): Put this elsewhere.
void handle_timer(regs* r) {
  if (r->int_no != 32) {
    fb_println("Error");
  }

  static int ticks = 0;
  ticks++;
  if (ticks % 100 == 0) {
    fb_println("tick");
  }
}

void handle_keyboard(regs* r) {
  if (r->int_no != 33) {
    fb_println("Error");
  }

  uint32_t scancode;
  scancode = inb(0x60);

  // The MSB of the scancode is whether or not the key was released.
  bool key_released = (scancode & 0x80);
  scancode &= ~0x80;  // Clear the flag.

  char key_name[] = "?";
  key_name[0] = kbdus[scancode];
  fb_println("keypress[%d] %s: %s",
	     scancode, (key_released ? "RELEASED" : "PRESSED"),
	     key_name);
}

// Handles IRQs. The mechanism is the same for interrupts, but we use a separate
// function to send the "End of Interrupt" command (0x20) to hardware.
void irq_handler(regs* r) {
  int irq_no = r->int_no - 32;

  // IRQ 8-15 need to send an End of Interrupt to slave controller at IDT 32.
  if (irq_no >= 8) {
    outb(0xA0, 0x20);
  }

  if (irq_no == 0) {
    handle_timer(r);
  }
  if (irq_no == 1) {
    handle_keyboard(r);
  }

  // Send "End of Interrupt"
  outb(0x20, 0x20);
}
