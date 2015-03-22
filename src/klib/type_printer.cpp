#include "klib/type_printer.h"

#include "klib/argaccumulator.h"
#include "klib/types.h"

namespace {

const char kInvalidType[] = "[ERROR: Invalid type]";

const char kHexDigits[] = "0123456789ABCDEF";

template<typename T>
void OutputHex(const T x, const uint8 digits_so_far, klib::IOutputFn* out) {
  if (digits_so_far == 0) {
    out->Print('0');
    out->Print('x');
  }
  if (digits_so_far >= sizeof(T) * 2) {
    return;
  }
  int digit_idx = (x & 0b1111);
  OutputHex(x / 16, digits_so_far + 1, out);
  out->Print(kHexDigits[digit_idx]);
}

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
    OutputHex(arg.value.i32, 0, out_);
    break;
  case ArgType::UINT32:
    OutputHex(arg.value.ui32, 0, out_);
    break;
  case ArgType::INT64:
    OutputHex(arg.value.i64, 0, out_);
    break;
  case ArgType::UINT64:
    OutputHex(arg.value.ui64, 0, out_);
    break;
  default:
    out_->Print(kInvalidType);
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

}  // namespace klib
