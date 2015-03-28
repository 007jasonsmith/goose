#include "klib/print.h"

#include "klib/argaccumulator.h"
#include "klib/strings.h"
#include "klib/type_printer.h"

namespace {

const char kErrorUnknownFormat[] = "[Error: Unknown format specifier]";
const char kErrorMissingFormat[] = "[Error: missing format specifier]";
const char kErrorArgsUnderspecified[] = "[Error: args underspecified]";
const char kErrorJustificationParse[] = "[Error: justification parse error]";

void PrintArg(klib::Arg arg, char format, klib::IOutputFn* out) {
  klib::TypePrinter tp(out);
  switch (format) {
  case 'c':
    tp.PrintChar(arg);
    break;
  case 'd':
    tp.PrintDec(arg);
    break;
  case 'h':
    tp.PrintHex(arg);
    break;
  case 's':
    tp.PrintString(arg);
    break;
  default:
    out->Print(kErrorUnknownFormat);
  }
}

struct JustifiedInfo {
  char alignment;
  size range;
  size index;  // Index where the parsing stopped.
};

bool TryParseJustifiedInfo(const char* format, JustifiedInfo* info) {
  // Assert at start of justification info.
  if (format[0] != '{') {
    info->index = 0;
    return false;
  }

  info->alignment = format[1];
  // Assert alignment is legal.
  switch (info->alignment) {
  case 'L': case 'C': case 'R':
    break;
  default:
    info->index = 1;
    return false;
  }

  size range = 0;
  size i = 2;
  while (format[i] >= '0' && format[i] <= '9') {
    size value = (format[i] - '0');
    range = range * 10 + value;
    i++;
  }
  // Assert non-zero range, assert some digits specified.
  if (range == 0) {
    info->index = i;
    return false;
  }
  info->range = range;

  // Assert at end of frange format.
  if (format[i] != '}') {
    info->index = i;
    return false;
  }

  info->index = i + 1;
  return true;
}

void PrintSpaces(size count, klib::IOutputFn* out) {
  for (size i = 0; i < count; i++) {
    out->Print(' ');
  }
}

// E.g. "{L20}s...", Arg.Of("leftaligned"), ...
int PrintJustified(const char* format, klib::Arg arg, klib::IOutputFn* out) {
  JustifiedInfo info;
  bool ok = TryParseJustifiedInfo(format, &info);
  if (!ok) {
    out->Print(kErrorJustificationParse);
    return info.index;
  }
  
  klib::StringPrinter sp;
  PrintArg(arg, format[info.index], &sp);

  size len = klib::length(sp.Get());
  size padding = info.range - len;
  if (padding <= 0) {
    out->Print(sp.Get());
  } else {
    switch (info.alignment) {
    case 'L':
      out->Print(sp.Get());
      PrintSpaces(padding, out);
      break;
    case 'C':
      PrintSpaces(padding / 2, out);
      out->Print(sp.Get());
      PrintSpaces((padding + 1)/ 2, out);  // +1 for odd ranges.
      break;
    case 'R':
      PrintSpaces(padding, out);
      out->Print(sp.Get());
      break;
    }
  }

  return info.index;
}

}  // anonymous namespace

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
	out->Print(kErrorArgsUnderspecified);
	return;
      }

      char next = format[i + 1];
      if (next == 0) {
	out->Print(kErrorMissingFormat);
	return;
      }

      if (next == '{') {
	int length = PrintJustified(&format[i+1], args.Get(arg_index), out);
	i += length;  // Advance to the end of the justification format.
      } else {
	PrintArg(args.Get(arg_index), next, out);
      }
      
      arg_index++;
      i++;  // Skip over next (the format specifier).
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
