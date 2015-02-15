#ifndef SYS_IO_H_
#define SYS_IO_H_

#include "lib/types.h"

// NOTE: These are C-stubs for functions written in assembly,
// see io.s.

/**
 * outb:
 *  Sends the given data to the given I/O port.
 *
 *  @param port The I/O port to send the data to.
 *  @param data The data to send to the I/O port
 */
void outb(uint32_t port, uint32_t data);

 /**
  * inb:
  *  Read a byte from an I/O port.
  *
  *  @param  port The address of the I/O port.
  *  @return      The read byte
  */
unsigned char inb(uint32_t port);

#endif  // SYS_IO_H_
