#include "klib/panic.h"

#include "klib/print.h"
#include "klib/type_printer.h"

namespace {

void (*panic_fn)(const char* message) = nullptr;

}  // anonymous namespace

namespace klib {

void AssertImpl(const char* expr, const char* file, int line) {
  StringPrinter sp;
  klib::Print("Assertion failure: \"%s\" in \"%s\":%d", &sp,
              expr, file, line);
  Panic(sp.Get());
}

void SetPanicFn(void (*fn)(const char* message)) {
  if (panic_fn != nullptr) {
    panic_fn("Attempt to set panic function after it has been defined.");
  }
  panic_fn = fn;
}

void Panic(const char* message) {
  if (panic_fn != nullptr) {
    panic_fn(message);
  }
}

}  // namespace klib
