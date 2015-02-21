#ifndef KLIB_BASE_H_
#define KLIB_BASE_H_

#include "lib/types.h"

// THIS WILL TOTALLY BREAK IF THE KERNEL'S CALLING CONVENTION CHANGES.
// TODO(chris): Create a unit test for this.
typedef unsigned char *va_list;
#define va_start(list, param) (list = (((va_list)&param) + sizeof(param)))
#define va_arg(list, type)    (*(type *)((list += sizeof(type)) - sizeof(type)))
#define va_end(list)          {} /* noop */

typedef void(*OutputFn)(char);

// Processes printf-style functions, using the provided
// function to actually output each character.
//
// You must call va_start to initialize args. va_end will NOT
// be called.
// TODO(chris): base_printf(const char* msg, OutputFn fn);
void base_printf(const char* msg, OutputFn fn, ...);

#endif  // KLIB_BASE_H_
