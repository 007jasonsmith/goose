#include "klib/type_printer.h"

#include "klib/argaccumulator.h"
#include "klib/limits.h"
#include "klib/macros.h"
#include "klib/types.h"

namespace {
const char kInvalidType[] = "[ERROR: Invalid type]";

// Hack to work around compiler warning. When we check if "x < 0" inside of
// PrintDec, we get a warning that for unsigned types the expression will
// always evaluate to false.
template <typename T, bool is_signed>
struct IsNegative{ 
  bool operator()(const T& x) {
    return x < 0;
  }
}; 

template <typename T>
struct IsNegative<T, false /* unsigned */>{ 
  bool operator()(const T x) {
    SUPPRESS_UNUSED_WARNING(x)
    return false;
  }
}; 

template<typename T>
void OutputDec(T x, klib::IOutputFn* out) {
  if (IsNegative<T, Limits<T>::IsSigned>()(x)) {
    out->Print('-');

    // We need to do extra shenanigans here, since -1 * MinInt overflows.
    if (x == Limits<T>::Min) {
      int digit = x % 10;
      OutputDec(x / -10, out);
      out->Print('0' + -digit);
      return;
    } else {
      x *= -1;
    }
  }
  
  int digit = x % 10;
  if (x >= 10) {
    OutputDec(x / 10, out);
  }
  out->Print('0' + digit);
}

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

template<typename T>
void OutputBin(const T x, uint8 bits_so_far, klib::IOutputFn* out) {
  if (bits_so_far == 0) {
    out->Print('0');
    out->Print('b');
  }
  if (bits_so_far >= 8 * sizeof(x)) {
    return;
  }

  int bit = (x & 1);
  OutputBin(x >> 1, bits_so_far + 1, out);
  out->Print(bit == 1 ? '1' : '0');
}

}  // anonymous namespace

namespace klib {

void IOutputFn::Print(const char* msg) {
  while (*msg) {
    Print(*msg);
    msg++;
  }
}

StringPrinter::StringPrinter() : index_(0), max_size_(kBufferSize) {
  Reset();
}

void StringPrinter::Print(char c) {
  if (index_ >= max_size_) {
    return;
  }
  buffer_[index_] = c;
  index_++;
}

const char* StringPrinter::Get() {
  return buffer_;
}

void StringPrinter::SetMaxSize(size new_size) {
  if (new_size < kBufferSize) {
    max_size_ = new_size;
  }
  if (new_size <= index_) {
    index_ = new_size;
  }
  // Nuke existing data. (Where new_size < previous size.)
  for (size i = new_size; i < kBufferSize; i++) {
    buffer_[i] = 0;
  }
}

void StringPrinter::Reset() {
  index_ = 0;
  for (size i = 0; i < kBufferSize; i++) {
    buffer_[i] = 0;
  }
  // Buffer holds one more char for a null, so
  // strings can be up to kBufferSize.
  buffer_[kBufferSize] = 0;
}

TypePrinter::TypePrinter(IOutputFn* out) : out_(out) {
  // TODO(chris): CHECK_NOTNULL(out_);
}

void TypePrinter::PrintDefault(Arg arg) {
  switch (arg.type) {
  case ArgType::CHAR:
    out_->Print(arg.value.c);
    break;
  case ArgType::INT32:
    OutputDec(arg.value.i32, out_);
    break;
  case ArgType::UINT32:
    OutputDec(arg.value.ui32, out_);
    break;
  case ArgType::INT64:
    OutputHex(arg.value.i64, 0, out_);
    break;
  case ArgType::UINT64:
    OutputHex(arg.value.ui64, 0, out_);
    break;
  case ArgType::CSTR:
    out_->Print(arg.value.cstr);
    break;
  default:
    out_->Print(kInvalidType);
  }
}

void TypePrinter::PrintChar(Arg arg) {
  switch (arg.type) {
  case ArgType::CHAR:
    out_->Print(arg.value.c);
    break;
  default:
    out_->Print(kInvalidType);
  }
}

void TypePrinter::PrintDec(Arg arg) {
  switch (arg.type) {
  case ArgType::INT32:
    OutputDec(arg.value.i32, out_);
    break;
  case ArgType::UINT32:
    OutputDec(arg.value.ui32, out_);
    break;
  // case ArgType::INT64:
  // case ArgType::UINT64:
  // 64-bit decimal printing requires __moddi3 and __divdi3 to be defined,
  // which are currently NYI in klib.
  default:
    out_->Print(kInvalidType);
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

void TypePrinter::PrintBin(Arg arg) {
  switch (arg.type) {
  // TODO(chris): Support signed integers too.
  case ArgType::UINT32:
    OutputBin(arg.value.ui32, 0, out_);
    break;
  case ArgType::UINT64:
    OutputBin(arg.value.ui64, 0, out_);
    break;
  default:
    out_->Print(kInvalidType);
  }
}

void TypePrinter::PrintString(Arg arg) {
  switch (arg.type) {
  case ArgType::CSTR:
    out_->Print(arg.value.cstr);
    break;
  default:
    out_->Print(kInvalidType);
  }
}

}  // namespace klib
