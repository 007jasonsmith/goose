// Routines to aid in the debugging of Goose. Writing to the debug log is not
// guaranteed to be in-order, regularly flushed, or anything that would make
// it especially useful. All it does is marshal text to the registered
// IOOutputFn.

#ifndef KLIB_DEBUG_H_
#define KLIB_DEBUG_H_

#include "klib/print.h"
#include "klib/type_printer.h"

namespace klib {

class Debug {
 private:
  // Do not use. Static utility class.
  Debug();

 public:
  static void RegisterOutputFn(IOutputFn* fn);
  static void Log(const char* msg);
  static void LogChar(char c);

  template<typename... Args>
  static void Log(const char* msg, Args... args) {
    // TODO(chris): CHECK fn_ not null.
    klib::Print(msg, fn_, args...);
    klib::Print("\n", fn_);
  }

 private:
  // We do not own.
  static IOutputFn* fn_;
};

}  // namespace klib

#endif  // KLIB_DEBUG_H_
