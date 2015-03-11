#include <string>
#include "gtest/gtest.h"

#include "klib/types.h"

// Suppress compiler warning for an unused variable.
#define SUPPRESS_UNUSED_WARNING(x) \
  (void) (x);

enum class ArgType { CHAR, CSTR, INT32, UINT32, INT64, UINT64 };

union ArgValue {
  char c;
  const char* cstr;
  int32 i32;
  uint32 ui32;
  int64 i64;
  uint64 ui64;
};

struct Arg {
  ArgType type;
  ArgValue value;

  #define OF(type1, type2, value_name) \
  static Arg Of(const type1 x) {       \
    Arg arg;                           \
    arg.type = ArgType::type2;         \
    arg.value.value_name = x;          \
    return arg;                        \
  }
  OF(char, CHAR, c)
  OF(char*, CSTR, cstr)
  OF(int32, INT32, i32)
  OF(uint32, UINT32, ui32)
  OF(int64, INT64, i64)
  OF(uint64, UINT64, ui64)
  #undef OF
};

class ArgAccumulator {
public:
  static ArgAccumulator Parse() {
    ArgAccumulator acc;
    return acc;
  }

  template<typename... Args>
  static ArgAccumulator Parse(Args... args) {
    ArgAccumulator acc;
    ArgAccumulator::Accumulate(&acc, args...);
    return acc;
  }

  Arg Get(int idx) {
    return args_[idx];
  }

  int Count() {
    return count_;
  }

private:
  explicit ArgAccumulator() : count_(0) {}

  #define ADD(type1, type2, value_name)  \
  void Add(const type1 x) {              \
    args_[count_].type = ArgType::type2; \
    args_[count_].value.value_name = x;  \
    count_++;                            \
  }
  ADD(char, CHAR, c)
  ADD(char*, CSTR, cstr)
  ADD(int32, INT32, i32)
  ADD(uint32, UINT32, ui32)
  ADD(int64, INT64, i64)
  ADD(uint64, UINT64, ui64)
  #undef ADD

  static void Accumulate(ArgAccumulator* accumulator) {
    SUPPRESS_UNUSED_WARNING(accumulator)
  }

  template<typename T, typename... Args>
  static void Accumulate(ArgAccumulator* accumulator, T value, Args... args) {
    accumulator->Add(value);
    ArgAccumulator::Accumulate(accumulator, args...);
  }

  // TODO(chris): Error if there are more than X args.
  int count_;
  Arg args_[10];
};

// TODO(chris): Test max number of args (> 10).
TEST(ArgAccumulator, NoArgs) {
  ArgAccumulator acc = ArgAccumulator::Parse();
  EXPECT_EQ(acc.Count(), 0);
}

TEST(ArgAccumulator, Char) {
  ArgAccumulator acc = ArgAccumulator::Parse('c');
  EXPECT_EQ(acc.Count(), 1);

  Arg arg = acc.Get(0);
  EXPECT_EQ(arg.type, ArgType::CHAR);
  EXPECT_EQ(arg.value.c, 'c');
}

TEST(ArgAccumulator, CStr) {
  ArgAccumulator acc = ArgAccumulator::Parse("foo");
  EXPECT_EQ(acc.Count(), 1);

  Arg arg = acc.Get(0);
  EXPECT_EQ(arg.type, ArgType::CSTR);
  EXPECT_STREQ(arg.value.cstr, "foo");
}

TEST(ArgAccumulator, Int32) {
  ArgAccumulator acc = ArgAccumulator::Parse(0xffff);
  EXPECT_EQ(acc.Count(), 1);

  Arg arg = acc.Get(0);
  EXPECT_EQ(arg.type, ArgType::INT32);
  EXPECT_EQ(arg.value.i32, 0xffff);
}

TEST(ArgAccumulator, UInt32) {
  uint32 ui = 0xffff;
  ArgAccumulator acc = ArgAccumulator::Parse(ui);
  EXPECT_EQ(acc.Count(), 1);

  Arg arg = acc.Get(0);
  EXPECT_EQ(arg.type, ArgType::UINT32);
  EXPECT_EQ(arg.value.ui32, ui);
}

TEST(ArgAccumulator, Int64) {
  int64 i = 0xffffffffffffffff;
  ArgAccumulator acc = ArgAccumulator::Parse(i);
  EXPECT_EQ(acc.Count(), 1);

  Arg arg = acc.Get(0);
  EXPECT_EQ(arg.type, ArgType::INT64);
  EXPECT_EQ(arg.value.i64, i);
}

TEST(ArgAccumulator, UInt64) {
  uint64 ui = 0xffffffffffffffff;
  ArgAccumulator acc = ArgAccumulator::Parse(ui);
  EXPECT_EQ(acc.Count(), 1);

  Arg arg = acc.Get(0);
  EXPECT_EQ(arg.type, ArgType::UINT64);
  EXPECT_EQ(arg.value.ui64, ui);
}

class IOutputFn {
 public:
  virtual void Print(char c) = 0;
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
    // LOL, -1*MIN_INT32 == BOOM.
    if (x == MIN_INT32) {
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
  tp.Print(Arg::Of(MAX_INT32));
  tp.Print(Arg::Of(" min:"));
  tp.Print(Arg::Of(MIN_INT32));
  EXPECT_STREQ(p.Get(), "max:2147483647 min:-2147483648");
}

TEST(TypePrinter, MaxUInt32s) {
  TestPrinter p;
  TypePrinter tp(&p);

  tp.Print(Arg::Of("max:"));
  tp.Print(Arg::Of(MAX_UINT32));
  tp.Print(Arg::Of(" min:"));
  tp.Print(Arg::Of(MIN_UINT32));
  EXPECT_STREQ(p.Get(), "max:4294967295 min:0");
}

TEST(TypePrinter, HexInt32s) {
  TestPrinter p;
  TypePrinter tp(&p);

  tp.PrintHex(Arg::Of(MAX_INT32));
  tp.Print(Arg::Of(' '));
  tp.PrintHex(Arg::Of(MAX_UINT32));
  tp.Print(Arg::Of(' '));
  tp.PrintHex(Arg::Of(0));
  EXPECT_STREQ(p.Get(), "0x7FFFFFFF 0xFFFFFFFF 0x00000000");
}

TEST(TypePrinter, HexInt64s) {
  TestPrinter p;
  TypePrinter tp(&p);

  tp.PrintHex(Arg::Of(MAX_INT64));
  tp.Print(Arg::Of(' '));
  tp.PrintHex(Arg::Of(MIN_INT64));
  tp.Print(Arg::Of(' '));
  tp.PrintHex(Arg::Of(MAX_UINT64));
  EXPECT_STREQ(
      p.Get(), "0x7FFFFFFFFFFFFFFF 0x8000000000000000 0xFFFFFFFFFFFFFFFF");
}

TEST(TypePrinter, HexError) {
  TestPrinter p;
  TypePrinter tp(&p);

  tp.PrintHex(Arg::Of("a cstr"));
  EXPECT_STREQ(p.Get(), "[ERROR: invalid type for hex]");
}

// TODO(chris): Tests for new printf and friends.
// - Test underflow
// - Test overflow
// - Test no args
// - Test many args

int main (int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
