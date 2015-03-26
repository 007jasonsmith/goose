#include "klib/strings.h"

#include "klib/types.h"

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
