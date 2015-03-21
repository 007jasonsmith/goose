#ifndef SYS_HALT_H_
#define SYS_HALT_H_

// Halt the system. Once called nothing can possibly happen again until
// the computer gets restarted.
extern "C" {

void system_halt(void);

}

#endif  // SYS_HALT_H_
