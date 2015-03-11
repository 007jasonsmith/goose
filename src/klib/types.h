// The kernel is compiled without standard C libraries, so unfortunately
// stdint.h is unavailable. This file contains a subset of the definitions,
// and will PROBABLY BREAK IF MOVED TO A DIFFERENT COMPILER.
//
// To the future person tasked with fixing this, I appologize in advance.
// --Chris Smith, 2/20/2015

#ifndef KLIB_TYPES_H_
#define KLIB_TYPES_H_

// TODO(chris): Do whatever compile-time magic you can do to
// ensure these are factually correct.
typedef char int8;
typedef unsigned char uint8;

typedef short int16;
typedef unsigned short uint16;

typedef int int32;
typedef unsigned int uint32;

typedef long long int int64;
typedef unsigned long long int uint64;

#define MAX_INT32 int32(2147483647)
#define MIN_INT32 int32(-2147483648)

#define MAX_UINT32 uint32(4294967295)
#define MIN_UINT32 uint32(0)

#define MAX_INT64 int64(9223372036854775807LL)
#define MIN_INT64 int64(-9223372036854775808LL)
#define MAX_UINT64 uint64(0xFFFFFFFFFFFFFFFFULL)
#define MIN_UINT64 uint64(0ULL)

// size is used as an index into any form of array. It is defined such that
// it will work even if the raw pointer size changes.
typedef uint32 size;

// raw_ptr is used as a pointer into memory. It is defined such that
// it will work even if the raw pointer size changes.
typedef uint32* raw_ptr;

#define null ((void*) 0)

#endif  // KLIBYPES_H_
