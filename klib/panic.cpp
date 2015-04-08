#include "klib/panic.h"

namespace {

void (*panic_fn)(const char* message) = nullptr;

}  // anonymous namespace

namespace klib {

void Assert(bool expr) {
  Assert(expr, "Assertion failure.");
}

void Assert(bool expr, const char* message) {
  if (!expr) {
    Panic(message);
  }
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
