/**
 * Basic debugging functionality. Currently this works by writing
 * messages to COM1, which will be written to disk by bochs.
 */

/**
 * Write a NULL-terminated string to the debugging log.
 */
void debug_log(const char* buf, ...);
