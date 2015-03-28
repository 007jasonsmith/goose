#include "klib/print.h"

#include "klib/argaccumulator.h"
#include "klib/type_printer.h"

namespace klib {

void Print(const char* format, const ArgAccumulator& args, IOutputFn* out) {
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
