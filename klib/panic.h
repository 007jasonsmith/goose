// Register a function to execute in the event of a kernel panic.

#ifndef KLIB_PANIC_H_
#define KLIB_PANIC_H_

#define Assert(EXPR)                             \
  if (!(EXPR)) {                                 \
    klib::AssertImpl(#EXPR, __FILE__, __LINE__); \
  }


namespace klib {

void AssertImpl(const char* expr, const char* file, int line);

// Sets the kernel panic function. Panics if already set.
void SetPanicFn(void (*fn)(const char* message));

// Trigger a kernel panic. Does nothing if SetPanicFn not called.
void Panic(const char* message);

}  // namespace klib

#endif
