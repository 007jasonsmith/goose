#include "gtest/gtest.h"

#include "klib/argaccumulator.h"
#include "klib/typeprinter.h"
#include "klib/types.h"

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
