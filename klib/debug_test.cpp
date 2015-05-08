#include "gtest/gtest.h"

#include "klib/debug.h"
#include "klib/type_printer.h"

namespace klib {

TEST(Debug, Basic) {
  StringPrinter output;
  Debug::RegisterOutputFn(&output);
  Debug::Log("testing...");
  EXPECT_STREQ(output.Get(), "testing...\n");

  output.Reset();
  Debug::Log("testing %d", 42);
  EXPECT_STREQ(output.Get(), "testing 42\n");

}

}  // namespace klib
