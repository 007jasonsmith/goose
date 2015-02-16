#ifndef LIB_PRINTF_H_
#define LIB_PRINTF_H_

#include "lib/types.h"

typedef void(*OutputFn)(char);


// Processes printf-style functions, using the provided
// function to actually output each character.
//
// You must call va_start to initialize args. va_end will NOT
// be called.
void base_printf(const char* msg, va_list args, OutputFn fn);
void print_va(const char* msg, OutputFn fn, ...);
void print_str(const char* msg, OutputFn fn);
void print_int(int x, OutputFn fn);
void print_hex(int x, OutputFn fn);
void print_bin(int x, OutputFn fn);

#endif  // LIB_PRINTF_H_
