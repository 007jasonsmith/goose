// The kernel is compiled without standard C libraries, so unfortunately
// stdint.h is unavailable. This file contains a subset of the definitions,
// and will PROBABLY BREAK IF MOVED TO A DIFFERENT COMPILER. To the future
// person tasked with fixing this, I appologize in advance.

#ifndef FIXED_TYPES_H_
#define FIXED_TYPES_H_

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

#endif  // FIXED_TYPES_H_
