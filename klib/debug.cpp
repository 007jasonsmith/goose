#include "klib/debug.h"

#include "klib/types.h"

namespace klib {

IOutputFn* Debug::fn_ = nullptr;

Debug::Debug() {}

void Debug::RegisterOutputFn(IOutputFn* fn) {
  // TODO(chrsmith): CHECK_NOTNULL(fn);
  // TODO(chrsmith): CHECK_NULL(fn_);
  fn_ = fn;
}

void Debug::Log(const char* msg) {
  // TODO(chrsmith): CHECK_NOTNULL(fn_);
  fn_->Print(msg);
  fn_->Print('\n');
}

}  // namespace klib
