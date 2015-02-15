// The kernel is compiled without standard C libraries, so unfortunately
// stdint.h is unavailable. This file contains a subset of the definitions,
// and will PROBABLY BREAK IF MOVED TO A DIFFERENT COMPILER. To the future
// person tasked with fixing this, I appologize in advance.

#ifndef LIB_TYPES_H_
#define LIB_TYPES_H_

// TODO(chris): Do whatever compile-time magic you can do to
// ensure these are factually correct.
typedef char int8_t;
typedef unsigned char uint8_t;

typedef short int16_t;
typedef unsigned short uint16_t;

typedef short int32_t;
typedef unsigned short uint32_t;

typedef int8_t bool;
#define true 1
#define false 0

typedef int size_t;

// Variadic argument
//
// THIS WILL TOTALLY BREAK IF THE KERNEL'S CALLING CONVENTION CHANGES.
// TODO(chris): Create a unit test for this.
typedef unsigned char *va_list;
#define va_start(list, param) (list = (((va_list)&param) + sizeof(param)))
#define va_arg(list, type)    (*(type *)((list += sizeof(type)) - sizeof(type)))
#define va_end(list)          {} /* noop */

#endif  // LIB_TYPES_H_
