#include "gtest/gtest.h"

#include "klib/argaccumulator.h"
#include "klib/type_printer.h"
#include "klib/types.h"

namespace klib {

// Test class wrapping an OutputFn.
// TODO(chris): Prevent duplication with new_printf_test.cpp
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

}
