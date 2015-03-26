#ifndef KLIB_STRINGS_H_
#define KLIB_STRINGS_H_

#include "klib/types.h"

namespace klib {

size length(const char* buf);
bool equal(const char* lhs, const char* rhs);

}  // namespace klib

#endif  // KLIB_STRINGS_H_
