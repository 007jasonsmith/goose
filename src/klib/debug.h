// Routines to aid in the debugging of Goose. Nothing here should be taken as
// a dependency, nor relied upon to not change.

#ifndef KLIB_DEBUG_H_
#define KLIB_DEBUG_H_

// Write a message to the "system log", which is currently COM1, which is
// written to disk by bochs.
// void debug_log(const char* msg, ...);
void debug_log(const char* msg);

#endif  // KLIB_DEBUG_H_
