// Printf, huzzah!

#ifndef KLIB_PRINT_H_
#define KLIB_PRINT_H_

#include "klib/argaccumulator.h"
#include "klib/type_printer.h"

namespace klib {

template<typename... Args>
void print(const char* format, IOutputFn* out, Args... func_args) {
  ArgAccumulator args = ArgAccumulator::Parse(func_args...);
  TypePrinter tp(out);

  int i = 0;
  int arg_index = 0;
  while (format[i] != 0) {
    char c = format[i];
    if (c != '%') {
      out->Print(c);
    } else {
      if (arg_index >= args.Count()) {
	out->Print("[Error: args underspecified]");
	return;
      }

      char next = format[i + 1];
      if (next == 0) {
	out->Print("[ERROR: missing format specifier]");
	return;
      }

      tp.Print(args.Get(arg_index));

      arg_index++;
      i++;  // Skip over next
    }
    i++;
  }

  while (arg_index < args.Count()) {
    out->Print(" Extra: ");
    tp.Print(args.Get(arg_index));
    arg_index++;
  }
}

}  // namespace klib

#endif  // KLIB_PRINT_
