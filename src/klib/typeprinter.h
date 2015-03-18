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

// Wrap a function for pinting characters. Used to provide
// higher-level primitives, such as printing values in hex,
// binary, etc.
//
// Errors encountered will be printed directly to fn.
class TypePrinter {
 public:
  explicit TypePrinter(IOutputFn* out);

  void Print(Arg arg);
  void PrintHex(Arg arg);

 private:
  void PrintCStr(const char* msg);

  void PrintDec(int32 x);
  void PrintDec(uint32 x);
  void PrintHex(int32 x, int digits_so_far);
  void PrintHex(uint32 x, int digits_so_far);
  void PrintHex(int64 x, int digits_so_far);
  void PrintHex(uint64 x, int digits_so_far);

  IOutputFn* out_;  // We do not own.
};

}  // namespace klib

#endif  // KLIB_TYPEPRINTER_H_
