// Type for printing types.

#ifndef KLIB_TYPEPRINTER_H_
#define KLIB_TYPEPRINTER_H_

#include "klib/argaccumulator.h"
#include "klib/types.h"

namespace klib {

class IOutputFn {
 public:
  virtual void Print(char c) = 0;
  void Print(const char* msg);
};

// Implementation of IOutputFn that prints to a fixed-width C-string.
// Output beyond the buffer size will be ignored.
class StringPrinter : public IOutputFn {
 public:
  StringPrinter();

  virtual void Print(char c);

  const char* Get();
  void Reset();

 private:
  const static size kBufferSize = 80;
  size index_;
  char buffer_[kBufferSize + 1];  // +1 for null.
};

// Wrap a function for pinting characters. Used to provide
// higher-level primitives, such as printing values in hex,
// binary, etc.
//
// Errors encountered will be printed directly to fn.
class TypePrinter {
 public:
  explicit TypePrinter(IOutputFn* out);

  void PrintDefault(Arg arg);
  void PrintChar(Arg arg);
  void PrintDec(Arg arg);
  void PrintHex(Arg arg);
  void PrintString(Arg arg);

 private:
  IOutputFn* out_;  // We do not own.
};

}  // namespace klib

#endif  // KLIB_TYPEPRINTER_H_
