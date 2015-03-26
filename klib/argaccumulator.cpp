#include "klib/argaccumulator.h"

#include "klib/macros.h"
#include "klib/types.h"

namespace klib {

ArgAccumulator ArgAccumulator::Parse() {
  ArgAccumulator acc;
  return acc;
}

Arg ArgAccumulator::Get(int idx) {
  return args_[idx];
}

size ArgAccumulator::Count() {
  return count_;
}

ArgAccumulator::ArgAccumulator() : count_(0) {}

void ArgAccumulator::Accumulate(ArgAccumulator* accumulator) {
  SUPPRESS_UNUSED_WARNING(accumulator)
}

}  // namespace klib
