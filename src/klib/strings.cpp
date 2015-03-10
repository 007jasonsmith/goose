#include "klib/strings.h"

#include "klib/types.h"

size str_length(const char* buf) {
  size i = 0;
  while(buf[i]) {
    i++;
  }
  return i;
}

bool str_compare(const char* lhs, const char* rhs) {
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
