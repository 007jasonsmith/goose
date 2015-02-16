#ifndef LIB_DEBUG_H_
#define LIB_DEBUG_H_

/**
 * Basic debugging functionality. Currently this works by writing
 * messages to COM1, which will be written to disk by bochs.
 */

#include "lib/types.h"

/**
 * Write a NULL-terminated string to the debugging log.
 */
void debug_log(const char* buf, ...);

/**
 * Dump an object's hex value to the log.
 */
void debug_log_obj(const char* name, void* obj, size_t bytes);

#endif  // LIB_DEBUG_H_
