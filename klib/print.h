// Printf, huzzah!

#ifndef KLIB_PRINT_H_
#define KLIB_PRINT_H_

#include "klib/argaccumulator.h"
#include "klib/type_printer.h"

namespace klib {

// Passable implementation of printf. It is strongly typed, so you need to match
// the right format specifiers to the right data type. (You'll get "[Error...]"
// printed if you don't.)
//
// %d - Print any integer (signed or unsigned) in base 10.
// %h - Print any integer (signed or unsigned) in hex.
// %c - Print a character.
// %s - Print a C-string.
//
// You can optionally align output. Output can be left, right, or center
// justified within a fixed text range. If the output is greater than the
// range, it will not be justified. Note that the text range cannot be greater
// than 80 characters.
//
// Example:
// %{L16}s - Print a string, left-justified padding up to 16 characters.
// %{R3}d  - Print an integer, right-justified padding up to 3 characters.
// %{C80}s - Print a string, center-justified in the middle of a range of 80.
template<typename... Args>
void Print(const char* format, IOutputFn* out, Args... func_args) {
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
	out->Print("[Error: missing format specifier]");
	return;
      }

      switch (next) {
      case 'c':
	tp.PrintChar(args.Get(arg_index));
	break;
      case 'd':
	tp.PrintDec(args.Get(arg_index));
	break;
      case 'h':
	tp.PrintHex(args.Get(arg_index));
	break;
      case 's':
	tp.PrintString(args.Get(arg_index));
	break;
      default:
	out->Print("[Error: Unknown format specifier]");
      }

      arg_index++;
      i++;  // Skip over next
    }
    i++;
  }

  while (arg_index < args.Count()) {
    out->Print(" Extra: ");
    tp.PrintDefault(args.Get(arg_index));
    arg_index++;
  }
}

}  // namespace klib

#endif  // KLIB_PRINT_
