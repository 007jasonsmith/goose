#include "gtest/gtest.h"

#include "klib/argaccumulator.h"
#include "klib/types.h"

namespace klib {

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

}  // namespace klib
