#include "gtest/gtest.h"

#include "klib/print.h"

namespace {

// Test class wrapping an OutputFn.
// TODO(chris): Prevent duplication with typeprinter_test.cpp
class TestPrinter : public klib::IOutputFn {
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

}  // anonymous namesapce

namespace klib {

// TODO(chris): Use a test framework to avoid all the instances of TestPrinter.

TEST(Printf, Basic) {
  TestPrinter p;
  print("hello, world!", &p);
  EXPECT_STREQ(p.Get(), "hello, world!");
}

TEST(Print, Basic2) {
  TestPrinter p;
  print("[%n] [%c] [%s]", &p, -11, '@', "hello, world");
  EXPECT_STREQ(p.Get(), "[-11] [@] [hello, world]");
}

TEST(Print, Extra) {
  TestPrinter p;
  print("%d", &p, 1, 2, 3);
  EXPECT_STREQ(p.Get(), "1 Extra: 2 Extra: 3");
}

TEST(Print, ErrorUnderspecified) {
  TestPrinter p;
  print("%d %d %d", &p, 1, 2);
  EXPECT_STREQ(p.Get(), "1 2 [Error: args underspecified]");
}

TEST(Print, ErrorOverspecified) {
  TestPrinter p;
  print("no args", &p, 1, 2, 3);
  EXPECT_STREQ(p.Get(), "no args Extra: 1 Extra: 2 Extra: 3");
}

}  // namespace klib
