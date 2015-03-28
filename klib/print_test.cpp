#include "gtest/gtest.h"

#include "klib/print.h"
#include "klib/type_printer.h"

namespace klib {

// TODO(chris): Use a test framework to avoid all the instances of StringPrinter.
TEST(Print, Basic) {
  StringPrinter p;
  Print("hello, world!", &p);
  EXPECT_STREQ(p.Get(), "hello, world!");
}

TEST(Print, Basic2) {
  StringPrinter p;
  Print("[%d] [%c] [%s]", &p, -11, '@', "hello, world");
  EXPECT_STREQ(p.Get(), "[-11] [@] [hello, world]");
}

TEST(Print, Extra) {
  StringPrinter p;
  Print("%d", &p, 1, 2, 3);
  EXPECT_STREQ(p.Get(), "1 Extra: 2 Extra: 3");
}

TEST(Print, ErrorUnderspecified) {
  StringPrinter p;
  Print("%d %d %d", &p, 1, 2);
  EXPECT_STREQ(p.Get(), "1 2 [Error: args underspecified]");
}

TEST(Print, ErrorOverspecified) {
  StringPrinter p;
  Print("no args", &p, 1, 2, 3);
  EXPECT_STREQ(p.Get(), "no args Extra: 1 Extra: 2 Extra: 3");
}

TEST(Print, ErrorUnknownSpecifier) {
  StringPrinter p;
  Print("%j", &p, 1);
  EXPECT_STREQ(p.Get(), "[Error: Unknown format specifier]");
}

TEST(PrintAlignment, Parsing) {
  StringPrinter p;
  Print("%{C2}s", &p, "parsed-ok");
  EXPECT_STREQ(p.Get(), "parsed-ok");
}

}  // namespace klib
