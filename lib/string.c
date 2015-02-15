#include "lib/string.h"

size_t strlen(const char* buf) {
  size_t i = 0;
  while(buf[i]) {
    i++;
  }
  return i;
}
