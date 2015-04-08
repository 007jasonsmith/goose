// Register a function to execute in the event of a kernel panic.

#ifndef KLIB_PANIC_H_
#define KLIB_PANIC_H_

namespace klib {

void Assert(bool expr);
void Assert(bool expr, const char* message);

// Sets the kernel panic function. Panics if already set.
void SetPanicFn(void (*fn)(const char* message));

// Trigger a kernel panic. Does nothing if SetPanicFn not called.
void Panic(const char* message);

}  // namespace klib

#endif
