// Type using the fancy, schmancy C++ variadic template arguments to
// aggregate parameters to a varaiadic function.

#ifndef KLIB_ARGACCUMULATOR_H_
#define KLIB_ARGACCUMULATOR_H_

#include "klib/types.h"

namespace klib {

enum class ArgType { CHAR, CSTR, INT32, UINT32, INT64, UINT64 };

union ArgValue {
  char c;
  const char* cstr;
  int32 i32;
  uint32 ui32;
  int64 i64;
  uint64 ui64;
};

struct Arg {
  #define OF(type1, type2, value_name) \
  static Arg Of(const type1 x) {       \
    Arg arg;                           \
    arg.type = ArgType::type2;         \
    arg.value.value_name = x;          \
    return arg;                        \
  }
  OF(char, CHAR, c)
  OF(char*, CSTR, cstr)
  OF(int32, INT32, i32)
  OF(uint32, UINT32, ui32)
  OF(int64, INT64, i64)
  OF(uint64, UINT64, ui64)
  #undef OF

  ArgType type;
  ArgValue value;
};

class ArgAccumulator {
 public:
  static ArgAccumulator Parse();

  template<typename... Args>
  static ArgAccumulator Parse(Args... args) {
    ArgAccumulator acc;
    ArgAccumulator::Accumulate(&acc, args...);
    return acc;
  }

  Arg Get(int idx);
  size Count();

 private:
  explicit ArgAccumulator();

  static void Accumulate(ArgAccumulator* accumulator);
  
  template<typename T, typename... Args>
  static void Accumulate(ArgAccumulator* accumulator, T value, Args... args) {
    accumulator->Add(value);
    ArgAccumulator::Accumulate(accumulator, args...);
  }

  #define ADD(type1, type2, value_name)  \
  void Add(const type1 x) {              \
    args_[count_].type = ArgType::type2; \
    args_[count_].value.value_name = x;  \
    count_++;                            \
  }
  ADD(char, CHAR, c)
  ADD(char*, CSTR, cstr)
  ADD(int32, INT32, i32)
  ADD(uint32, UINT32, ui32)
  ADD(int64, INT64, i64)
  ADD(uint64, UINT64, ui64)
  #undef ADD

  // TODO(chris): Error if there are more than X args.
  size count_;
  Arg args_[16];
};

}  // namespace klib

#endif  // KLIB_ARGACCUMULATOR_H_
