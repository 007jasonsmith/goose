// The kernel is compiled without standard C libraries, so unfortunately
// stdint.h is unavailable. This file contains a subset of the definitions,
// and will DEFINITELY BREAK IF MOVED TO A DIFFERENT COMPILER.
//
// To the future person tasked with fixing this, I appologize in advance.
// --Chris Smith, 2/20/2015

#ifndef KLIB_TYPES_H_
#define KLIB_TYPES_H_

typedef unsigned char byte;

typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int int64;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long int uint64;

#define TEST_SIZE(type, size)                   \
  static_assert(sizeof(type) == size,           \
                "sizeof(" #type ") != " #size);
TEST_SIZE(byte,  1)

TEST_SIZE(int8,  1)
TEST_SIZE(int16, 2)
TEST_SIZE(int32, 4)
TEST_SIZE(int64, 8)

TEST_SIZE(uint8,  1)
TEST_SIZE(uint16, 2)
TEST_SIZE(uint32, 4)
TEST_SIZE(uint64, 8)
#undef TEST_SIZE

// size is used as an index into any form of array. It is defined such that
// it will work even if the raw pointer size changes.
typedef int size;

// raw_ptr is used as a pointer into memory. It is defined such that
// it will work even if the raw pointer size changes.
typedef size* raw_ptr;

#endif  // KLIB_TYPES_H_
