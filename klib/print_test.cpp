#include "gtest/gtest.h"

#include "klib/print.h"
#include "klib/type_printer.h"

namespace {

struct PrintStringTestCase {
  const char* input;
  const char* arg;
  const char* want;
};

}  // anonyous namesapce

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
  Print("%{C9}s", &p, "parsed-ok");
  EXPECT_STREQ(p.Get(), "parsed-ok");

  p.Reset();
  Print("%{C9:t}s", &p, "parsed-ok");
  EXPECT_STREQ(p.Get(), "parsed-ok");

}

TEST(PrintAlignment, NotParsing) {
  StringPrinter p;
  Print("{C2}s", &p);
  EXPECT_STREQ(p.Get(), "{C2}s");
}

TEST(PrintAlignment, ParseError) {
  // Defined as a macro so we can concat two strings.
  #define PARSE_ERROR "[Error: justification parse error]"

  StringPrinter p;
  Print("%{X2}s", &p, "???");
  EXPECT_STREQ(p.Get(), PARSE_ERROR "2}s");

  p.Reset();
  Print("%{C-2}s", &p, "???");
  EXPECT_STREQ(p.Get(), PARSE_ERROR "2}s");

  p.Reset();
  Print("%{C0}s", &p, "???");
  EXPECT_STREQ(p.Get(), PARSE_ERROR "s");

  p.Reset();
  Print("%{C123Xs", &p, "???");
  EXPECT_STREQ(p.Get(), PARSE_ERROR "s");

  p.Reset();
  Print("%{C123:f}s", &p, "???");
  EXPECT_STREQ(p.Get(), PARSE_ERROR "}s");
}

TEST(PrintAlignment, LeftAligned) {
  PrintStringTestCase test_cases[] = {
    { "[%{L8}s]", "1234", "[1234    ]"},
    { "[%{L5}s]", "1234", "[1234 ]"},
    { "[%{L1}s]", "1234", "[1234]"}
  };
  const size num_test_cases = sizeof(test_cases) / sizeof(PrintStringTestCase);

  for (size i = 0; i < num_test_cases; i++) {
    PrintStringTestCase test = test_cases[i];

    StringPrinter p;
    Print(test.input, &p, test.arg);
    EXPECT_STREQ(p.Get(), test.want);
  }
}

TEST(PrintAlignment, CenterAligned) {
  PrintStringTestCase test_cases[] = {
    { "[%{C8}s]", "1234", "[  1234  ]"},
    { "[%{C10}s]", "1234", "[   1234   ]"},
    { "[%{C1}s]", "1234", "[1234]"},
    { "[%{C9}s]", "1234", "[  1234   ]"},
    { "[%{C5}s]", "1234", "[1234 ]"}
  };
  const size num_test_cases = sizeof(test_cases) / sizeof(PrintStringTestCase);

  for (size i = 0; i < num_test_cases; i++) {
    PrintStringTestCase test = test_cases[i];

    StringPrinter p;
    Print(test.input, &p, test.arg);
    EXPECT_STREQ(p.Get(), test.want);
  }
}

TEST(PrintAlignment, RightAligned) {
  PrintStringTestCase test_cases[] = {
    { "[%{R9}s]", "1234", "[     1234]"},
    { "[%{R7}s]", "1234", "[   1234]"},
    { "[%{R1}s]", "1234", "[1234]"},
  };
  const size num_test_cases = sizeof(test_cases) / sizeof(PrintStringTestCase);

  for (size i = 0; i < num_test_cases; i++) {
    PrintStringTestCase test = test_cases[i];

    StringPrinter p;
    Print(test.input, &p, test.arg);
    EXPECT_STREQ(p.Get(), test.want);
  }
}

TEST(PrintAlignment, Truncation) {
  PrintStringTestCase test_cases[] = {
    { "[%{L2:t}s]", "1234", "[12]"},
    { "[%{C2:t}s]", "1234", "[12]"},
    { "[%{R2:t}s]", "1234", "[12]"},
  };
  const size num_test_cases = sizeof(test_cases) / sizeof(PrintStringTestCase);

  for (size i = 0; i < num_test_cases; i++) {
    PrintStringTestCase test = test_cases[i];

    StringPrinter p;
    Print(test.input, &p, test.arg);
    EXPECT_STREQ(p.Get(), test.want);
  }
}

}  // namespace klib
