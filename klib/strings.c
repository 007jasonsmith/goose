#include "klib/strings.h"

#include "klib/types.h"

size str_length(const char* buf) {
  size i = 0;
  while(buf[i]) {
    i++;
  }
  return i;
}
