#include "lib/printf.h"

#include "lib/debug.h"
#include "lib/string.h"

void print_int(int x, OutputFn fn);

void base_printf(const char* msg, va_list args, OutputFn fn) {
  size_t len = strlen(msg);
  for (size_t i = 0; i < len; i++) {
    char c = msg[i];

    if (c != '%') {
      fn(c);
      continue;
    }

    // Was the percent escaped?
    if (i > 0 && msg[i - 1] == '\\') {
      fn('%');
      continue;
    }

    // Get the specifier.
    if (i == len - 1) {
      // TODO(chris): Panic, ending with '%'.
      fn('%');
      continue;
    }
    c = msg[i + 1];

    // TODO(chris): Put these into a union.
    int int_arg;
    char* char_ptr;

    switch (c) {
    case 'd':
      int_arg = va_arg(args, int);
      if (int_arg < 0) {
	fn('-');
	int_arg *= -1;
      }

      print_int(int_arg, fn);
      break;
    case 's':
      char_ptr = va_arg(args, char*);
      base_printf(char_ptr, args, fn);
      break;
    default:
      // TODO(chris): Panic.
      debug_log("Error: Unknown format specifier.");
    }
    // Skip the format specifier.
    i++;
 }
}

void print_int(int x, OutputFn fn) {
  int digit = x % 10;
  if (x >= 10) {
    print_int(x / 10, fn);
  }
  fn('0' + digit);
}
