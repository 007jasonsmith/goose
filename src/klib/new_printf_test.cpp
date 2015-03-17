#include <string>
#include "gtest/gtest.h"

#include "klib/argaccumulator.h"
#include "klib/types.h"
#include "klib/macros.h"

namespace klib {

class IOutputFn {
 public:
  virtual void Print(char c) = 0;

  void Print(const char* msg) {
    while (*msg) {
      Print(*msg);
      msg++;
    }
  }
};

// Wrap a function for pinting characters. Used to provide
// higher-level primitives, such as printing values in hex,
// binary, etc.
//
// Errors encountered will be printed directly to fn.
class TypePrinter {
 public:
  explicit TypePrinter(IOutputFn* out) : out_(out) {
    // TODO(chris): CHECK_NOTNULL(out_);
  }

  // Print the default value. e.g. base-10.
  void Print(Arg arg) {
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
      PrintCStr("TODO-support-int64");
      break;
    case ArgType::UINT64:
      PrintCStr("TODO-support-uint64");
      break;      
    default:
      PrintCStr("[ERROR: unknown type]");
    }
  }

  void PrintHex(Arg arg) {
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

 private:
  void PrintCStr(const char* msg) {
    while (*msg) {
      out_->Print(*msg);
      msg++;
    }
  }

  void PrintDec(int32 x) {
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

  void PrintDec(uint32 x) {
    int digit = x % 10;
    if (x >= 10) {
      PrintDec(x / 10);
    }
    out_->Print('0' + digit);
  }

  void PrintHex(int32 x, int digits_so_far) {
    const char kHexDigits[] = "0123456789ABCDEF";
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

  void PrintHex(uint32 x, int digits_so_far) {
    const char kHexDigits[] = "0123456789ABCDEF";
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

  void PrintHex(int64 x, int digits_so_far) {
    const char kHexDigits[] = "0123456789ABCDEF";
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

  void PrintHex(uint64 x, int digits_so_far) {
    const char kHexDigits[] = "0123456789ABCDEF";
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

  IOutputFn* out_;  // We do not own.
};

// Test class wrapping an OutputFn.
class TestPrinter : public IOutputFn {
 public:
  virtual void Print(char c) {
    msg_ += c;
  }

  const char* Get() {
    return msg_.c_str();
  }

  void Reset() {
    msg_ = "";
  }
 private:
  std::string msg_;
};

// TODO(chris): Use a test framework, so TestPrinter and TypePrinter are avail.
// TODO(chris): Print error case: invalid type.
// TODO(chris): PrintHex error case: invalid type.
TEST(TypePrinter, Basic) {
  TestPrinter p;
  TypePrinter tp(&p);

  tp.Print(Arg::Of(uint32(42)));
  tp.Print(Arg::Of('|'));
  tp.Print(Arg::Of(-42));
  EXPECT_STREQ(p.Get(), "42|-42");
}

TEST(TypePrinter, Chars) {
  TestPrinter p;
  TypePrinter tp(&p);

  tp.Print(Arg::Of('['));
  tp.Print(Arg::Of('\n'));
  tp.Print(Arg::Of('\t'));
  tp.Print(Arg::Of(' '));
  tp.Print(Arg::Of('@'));
  tp.Print(Arg::Of(']'));
  EXPECT_STREQ(p.Get(), "[\n\t @]");
}


TEST(TypePrinter, MaxInt32s) {
  TestPrinter p;
  TypePrinter tp(&p);

  tp.Print(Arg::Of("max:"));
  tp.Print(Arg::Of(kMaxInt32));
  tp.Print(Arg::Of(" min:"));
  tp.Print(Arg::Of(kMinInt32));
  EXPECT_STREQ(p.Get(), "max:2147483647 min:-2147483648");
}

TEST(TypePrinter, MaxUInt32s) {
  TestPrinter p;
  TypePrinter tp(&p);

  tp.Print(Arg::Of("max:"));
  tp.Print(Arg::Of(kMaxUInt32));
  tp.Print(Arg::Of(" min:"));
  tp.Print(Arg::Of(kMinUInt32));
  EXPECT_STREQ(p.Get(), "max:4294967295 min:0");
}

TEST(TypePrinter, HexInt32s) {
  TestPrinter p;
  TypePrinter tp(&p);

  tp.PrintHex(Arg::Of(kMaxInt32));
  tp.Print(Arg::Of(' '));
  tp.PrintHex(Arg::Of(kMaxUInt32));
  tp.Print(Arg::Of(' '));
  tp.PrintHex(Arg::Of(0));
  EXPECT_STREQ(p.Get(), "0x7FFFFFFF 0xFFFFFFFF 0x00000000");
}

TEST(TypePrinter, HexInt64s) {
  TestPrinter p;
  TypePrinter tp(&p);

  tp.PrintHex(Arg::Of(kMaxInt64));
  tp.Print(Arg::Of(' '));
  tp.PrintHex(Arg::Of(kMinInt64));
  tp.Print(Arg::Of(' '));
  tp.PrintHex(Arg::Of(kMaxUInt64));
  EXPECT_STREQ(
      p.Get(), "0x7FFFFFFFFFFFFFFF 0x8000000000000000 0xFFFFFFFFFFFFFFFF");
}

TEST(TypePrinter, HexError) {
  TestPrinter p;
  TypePrinter tp(&p);

  tp.PrintHex(Arg::Of("a cstr"));
  EXPECT_STREQ(p.Get(), "[ERROR: invalid type for hex]");
}

template<typename... Args>
void printf(const char* format, IOutputFn* out, Args... func_args) {
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

TEST(Printf, Basic) {
  TestPrinter p;
  printf("hello, world!", &p);
  EXPECT_STREQ(p.Get(), "hello, world!");
}

TEST(Printf, Basic2) {
  TestPrinter p;
  printf("[%n] [%c] [%s]", &p, -11, '@', "hello, world");
  EXPECT_STREQ(p.Get(), "[-11] [@] [hello, world]");
}

TEST(Printf, Extra) {
  TestPrinter p;
  printf("%d", &p, 1, 2, 3);
  EXPECT_STREQ(p.Get(), "1 Extra: 2 Extra: 3");
}

TEST(Printf, ErrorUnderspecified) {
  TestPrinter p;
  printf("%d %d %d", &p, 1, 2);
  EXPECT_STREQ(p.Get(), "1 2 [Error: args underspecified]");
}

TEST(Printf, ErrorOverspecified) {
  TestPrinter p;
  printf("no args", &p, 1, 2, 3);
  EXPECT_STREQ(p.Get(), "no args Extra: 1 Extra: 2 Extra: 3");
}

}  // namespace klib
