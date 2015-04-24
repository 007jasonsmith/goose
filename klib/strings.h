#ifndef KLIB_STRINGS_H_
#define KLIB_STRINGS_H_

#include "klib/types.h"

extern "C" {

// You shouldn't call this inefficient version of memcpy. Unfortunately clang
// requires it, since it replaces memory copy operations with calls to memcpy.
void *memcpy(void *dest, const void *src, size n);

}  // extern "C"


namespace klib {

size length(const char* buf);
bool equal(const char* lhs, const char* rhs);

}  // namespace klib

#endif  // KLIB_STRINGS_H_
