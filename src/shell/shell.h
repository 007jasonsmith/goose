// Rumedial shell.

#ifndef SHELL_SHELL_H_
#define SHELL_SHELL_H_

#include "hal/text_ui.h"
#include "klib/print.h"
#include "klib/type_printer.h"

namespace shell {

// Write-only stream for writing data to the shell. This will act as an
// intermediary between the writer and text mode. This way things like
// scrolling should happen seamlessly.
class ShellStream : public klib::IOutputFn {
 public:
  ShellStream(const hal::Region region, hal::Offset offset);

  virtual void Print(char c);
  hal::Offset Offset();

  template<typename... Args>
  void WriteLine(const char* msg, Args... args) {
    klib::Print(msg, this, args...);
    klib::Print("\n", this);
  }

  template<typename... Args>
  void Write(const char* msg, Args... args) {
    klib::Print(msg, this, args...);
  }

 private:
  const hal::Region region_;
  hal::Offset offset_;
};

// Starts running the shell.
void Run();

}  // namespace shell

#endif  // SHELL_SHELL_H_
