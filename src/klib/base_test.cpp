#include "gtest/gtest.h"

#include "klib/base.h"
#include "klib/types.h"

TEST(Base, Datatypes) {
  EXPECT_EQ(sizeof(int32), 4);
  EXPECT_EQ(sizeof(int64), 8);

  EXPECT_EQ(sizeof(uint32), 4);
  EXPECT_EQ(sizeof(uint64), 8);

  EXPECT_EQ(sizeof(size), 4);
}

const int kBufferSize = 128;
int chars_printed = 0;
char print_buffer[kBufferSize];

void ResetBuffer() {
  chars_printed = 0;
  for (int i = 0; i < kBufferSize; i++) {
    print_buffer[i] = '\0';
  }
}

void PrintChar(char c) {
  print_buffer[chars_printed] = c;
  chars_printed++;
}

TEST(BasePrintf, Basic) {
  ResetBuffer();
  base_printf("this is a test", &PrintChar);
  EXPECT_STREQ(print_buffer, "this is a test");
}

TEST(BasePrintf, Chars) {
  ResetBuffer();
  base_printf("test: %c", &PrintChar, 'g');
  //  base_printf("%c %c %c %c %c", &PrintChar, 'a', 'e', 'i', 'o', 'u');
  EXPECT_STREQ(print_buffer, "a e i o u");
}

int main (int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
