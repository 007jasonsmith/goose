Converting from GCC to Clang
4/24/2015

Clang is a better compiler, providing better errors, etc.

Conversion was straight forward. Since Goose currently uses Make, it was
simply a matter of replacing CC with /user/bin/clang.

Outcomes:

* Needed to provide an implementation of memcpy. Evidently Clang emits calls
  to memcpy as part of the compile, and evidently no way around this. Added
  in klib/strings.cpp.

However, there was what appears to be a compiler bug. For reals.

While Goose runs fine when built under GCC, when using Clang it would trigger
an interrupt whenever printing a string and translating format modifiers.

For example: Debug::Log("x = %d", 10); would print "x =" and then trigger
interrupt 6 (Invalid OpCode).

Abandoning Clang for the time being.
