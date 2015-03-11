#include <string>
#include "gtest/gtest.h"

#include "klib/types.h"

enum class ArgType { CHAR, CSTR, INT32 };

union ArgValue {
  char c;
  const char* cstr;
  int32 i32;
};

struct Arg {
  ArgType type;
  ArgValue value;
};

class ArgAccumulator {
public:
  template<typename... Args>
  static ArgAccumulator Parse(Args... args) {
    ArgAccumulator acc;
    acc_args(&acc, args...);
    return acc;
  }


  ArgAccumulator() : count_(0) {}

  #define ADD(type1, type2, value_name)  \
  void Add(const type1 x) {              \
    args_[count_].type = ArgType::type2; \
    args_[count_].value.value_name = x;  \
    count_++;                            \
  }
  ADD(char, CHAR, c)
  ADD(char*, CSTR, cstr)
  ADD(int32, INT32, i32)
  #undef ADD

  Arg Get(int idx) {
    return args_[idx];
  }

  int Count() {
    return count_;
  }

private:
  int count_;
  Arg args_[10];
};

void acc_args(ArgAccumulator* accumulator) {
  // Base case.
  (void) accumulator;  // Supress warning.
}

template<typename T, typename... Args>
void acc_args(ArgAccumulator* accumulator, T value, Args... args) {
  accumulator->Add(value);
  acc_args(accumulator, args...);
}

TEST(ArgAccumulator, NoArgs) {
  ArgAccumulator acc;
  acc_args(&acc);
  EXPECT_EQ(acc.Count(), 0);
}

TEST(ArgAccumulator, Char) {
  ArgAccumulator acc;
  acc_args(&acc, 'c');
  EXPECT_EQ(acc.Count(), 1);

  Arg arg = acc.Get(0);
  EXPECT_EQ(arg.type, ArgType::CHAR);
  EXPECT_EQ(arg.value.c, 'c');
}

TEST(ArgAccumulator, CStr) {
  ArgAccumulator acc;
  acc_args(&acc, "foo");
  EXPECT_EQ(acc.Count(), 1);

  Arg arg = acc.Get(0);
  EXPECT_EQ(arg.type, ArgType::CSTR);
  EXPECT_STREQ(arg.value.cstr, "foo");
}

TEST(ArgAccumulator, Int32) {
  ArgAccumulator acc;
  acc_args(&acc, 0xffff);
  EXPECT_EQ(acc.Count(), 1);

  Arg arg = acc.Get(0);
  EXPECT_EQ(arg.type, ArgType::INT32);
  EXPECT_EQ(arg.value.i32, 0xffff);
}

class TestPrinter {
 public:
  TestPrinter() : msg_("") {}

  void Print(char c) {
    msg_ += c;
  }

  void Print(const char* msg) {
    while (*msg) {
      msg_ += *msg;
      msg++;
    }
  }

  void Print(int x) {
    PrintInt(x);
  }

  const char* Get() {
    return msg_.c_str();
  }

  void Reset() {
    msg_ = "";
  }

 private:
  void PrintInt(int x) {
    int digit = x % 10;
    if (x >= 10) {
      PrintInt(x / 10);
    }
    msg_ += ('0' + digit);
  }

  std::string msg_;
};

void new_printf(const char* msg, TestPrinter* printer) {
  // TODO(chris): Account for existing % modifiers.
  while (*msg) {
    printer->Print(*msg);
    msg++;
  }
}

template<typename T, typename... Args>
void new_printf(const char *s, TestPrinter* printer, T value, Args... args) {
  while (*s) {
    if (*s == '%') {
      if (*(s + 1) == '%') {
	++s;
      } else {
	printer->Print(value);
	s += 2;
	new_printf(s, printer, args...);
	return;
      }
    }
    printer->Print(*s);
    s++;
  }    
}

TEST(BasePrintf, Basic) {
  TestPrinter p;
  new_printf("this is a test", &p);
  EXPECT_STREQ(p.Get(), "this is a test");
}

TEST(BasePrintf, Number) {
  TestPrinter p;
  new_printf("eleven [%d]", &p, 11);
  EXPECT_STREQ(p.Get(), "eleven [11]");
}

TEST(BasePrintf, String) {
  TestPrinter p;
  new_printf("alpha [%s]", &p, "alpha");
  EXPECT_STREQ(p.Get(), "alpha [alpha]");
}


template<typename... Args>
void newer_printf(const char *s, TestPrinter* printer, Args... args) {
  ArgAccumulator acc;
  acc_args(&acc, args...);
  printer->Print(acc.Count());
  (void)s;
}

template<typename... Args>
void newer_printf2(TestPrinter* printer, Args... args) {
  auto acc = ArgAccumulator::Parse(args...);
  printer->Print(acc.Get(0).value.i32);
  printer->Print(acc.Get(1).value.i32);
  printer->Print(acc.Get(2).value.i32);
}


TEST(NewerPrintf, Basic) {
  TestPrinter p;
  newer_printf("ASDF", &p);
  newer_printf("ASDF", &p, 1);
  newer_printf("ASDF", &p, 1, 2);
  EXPECT_STREQ(p.Get(), "012");
}

TEST(NewerPrintf, Adv) {
  TestPrinter p;
  newer_printf2(&p, 1, 2, 3);
  EXPECT_STREQ(p.Get(), "123");
  newer_printf2(&p, 4, 5, 6);
  EXPECT_STREQ(p.Get(), "123456");
}

int main (int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
