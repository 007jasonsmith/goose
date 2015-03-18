#include "klib/type_printer.h"

#include "klib/argaccumulator.h"
#include "klib/types.h"

namespace {
const char kHexDigits[] = "0123456789ABCDEF";
}  // anonymous namespace

namespace klib {

void IOutputFn::Print(const char* msg) {
  while (*msg) {
    Print(*msg);
    msg++;
  }
}

TypePrinter::TypePrinter(IOutputFn* out) : out_(out) {
  // TODO(chris): CHECK_NOTNULL(out_);
}

void TypePrinter::Print(Arg arg) {
  switch (arg.type) {
  case ArgType::CHAR:
    out_->Print(arg.value.c);
    break;
  case ArgType::CSTR:
    PrintCStr(arg.value.cstr);
      break;
  case ArgType::INT32:
    PrintDec(arg.value.i32);
    break;
  case ArgType::UINT32:
    PrintDec(arg.value.ui32);
    break;
  case ArgType::INT64:
    PrintCStr("TODO(chris): support-int64");
    break;
  case ArgType::UINT64:
    PrintCStr("TODO(chris): support-uint64");
    break;      
  default:
    PrintCStr("[ERROR: unknown type]");
  }
}

void TypePrinter::PrintHex(Arg arg) {
  switch (arg.type) {
  case ArgType::INT32:
    PrintHex(arg.value.i32, 0);
    break;
  case ArgType::UINT32:
    PrintHex(arg.value.ui32, 0);
    break;
  case ArgType::INT64:
      PrintHex(arg.value.i64, 0);
      break;
  case ArgType::UINT64:
    PrintHex(arg.value.ui64, 0);
    break;
  default:
    PrintCStr("[ERROR: invalid type for hex]");
  }
}

void TypePrinter::PrintCStr(const char* msg) {
  while (*msg) {
    out_->Print(*msg);
    msg++;
  }
}

void TypePrinter::PrintDec(int32 x) {
  // LOL, -1*kMinInt32 == BOOM.
  if (x == kMinInt32) {
    const char* digits = "-2147483648";
    while (*digits) {
      out_->Print(*digits);
      digits++;
    }
    return;
  }
  
  if (x < 0) {
    out_->Print('-');
    x *= -1;
  }
  
  int digit = x % 10;
  if (x >= 10) {
    PrintDec(x / 10);
  }
  out_->Print('0' + digit);
}
  
void TypePrinter::PrintDec(uint32 x) {
  int digit = x % 10;
  if (x >= 10) {
    PrintDec(x / 10);
  }
  out_->Print('0' + digit);
}
  
void TypePrinter::PrintHex(int32 x, int digits_so_far) {
  if (digits_so_far == 0) {
    out_->Print('0');
    out_->Print('x');
  }
  if (digits_so_far >= 8) {
    return;
  }
  int digit_idx = x % 16;
  PrintHex(x / 16, digits_so_far + 1);
  out_->Print(kHexDigits[digit_idx]);
}

void TypePrinter::PrintHex(uint32 x, int digits_so_far) {
  if (digits_so_far == 0) {
    out_->Print('0');
    out_->Print('x');
  }
  if (digits_so_far >= 8) {
    return;
  }
  int digit_idx = x % 16;
  PrintHex(x / 16, digits_so_far + 1);
  out_->Print(kHexDigits[digit_idx]);
}

void TypePrinter::PrintHex(int64 x, int digits_so_far) {
  if (digits_so_far == 0) {
    out_->Print('0');
    out_->Print('x');
  }
  if (digits_so_far >= 16) {
    return;
  }
  // int digit_idx = x % 16;
  int digit_idx = (x & 0b1111);
  PrintHex(x / 16, digits_so_far + 1);
    out_->Print(kHexDigits[digit_idx]);
}
  
void TypePrinter::PrintHex(uint64 x, int digits_so_far) {
  if (digits_so_far == 0) {
    out_->Print('0');
    out_->Print('x');
  }
  if (digits_so_far >= 16) {
    return;
  }
  int digit_idx = x % 16;
  PrintHex(x / 16, digits_so_far + 1);
    out_->Print(kHexDigits[digit_idx]);
}

}  // namespace klib
