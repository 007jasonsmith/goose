#include "klib/strings.h"

#include "klib/types.h"

extern "C" {

// TODO(chris): Copy size bytes at a time.
// TODO(chris): Copy on word boundaries.
// TODO(chris): Avoid mucking with overlapping regions.
void *memcpy(void *dest, const void *source, size n) {
  uint8* dest8 = (uint8*) dest;
  uint8* source8 = (uint8*) source;
  for (size i = 0; i < n; i++) {
    dest8[i] = source8[i];
  }
  return dest;
}

}  // extern "C"

namespace klib {

size length(const char* buf) {
  size i = 0;
  while(buf[i]) {
    i++;
  }
  return i;
}

bool equal(const char* lhs, const char* rhs) {
  while (true) {
    if (*lhs != *rhs) {
      return false;
    }
    if (*lhs == 0) {
      return true;
    }
    lhs++;
    rhs++;
  }
}

}  // namespace klib
