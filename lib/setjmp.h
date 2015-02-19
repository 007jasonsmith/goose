// Implemented as an execise. Not actually intended to be used.
// (There are better error-handling mechanisms, but not by much.)
#ifndef LIB_SETJMP_H_
#define LIB_SETJMP_H_

// http://en.wikipedia.org/wiki/Setjmp.h

// Registers when the ISR was triggered. Used for (hopefully) diagnosing bugs.
typedef struct __attribute((pack))__ {
  // Pushed the segments last.
  uint32_t gs;
  uint32_t fs;
  uint32_t es;
  uint32_t ds;
  uint32_t cs;

  // Pushed by "pusha".
  uint32_t edi;
  uint32_t esi;
  uint32_t ebp;
  uint32_t esp;

  uint32_t ebx;
  uint32_t edx;
  uint32_t ecx;
  uint32_t eax;

  // Pushed by the processor automatically.
  uint32_t eip;
  uint32_t cs;
  uint32_t eflags;
  uint32_t useresp;
  uint32_t ss;
} execution_state;

// NOT ANSI-C compliant.
typedef struct {
  execution_state state;
  int value;
} jmp_buf;

// Assembly functions for saving/restoring execution state.
void save_execution_state(execution_state* state);
void restore_execution_state(execution_state* state);

int setjmp(jmp_buf* env) {
  env->value = 0;
  save_execution_state(env);
  return env->value;
}

void longjmp(jmp_buf* env, int value) {
  env.value = value;
  restore_execution_state(env);
}

#endif  // LIB_SETJMP_H_
