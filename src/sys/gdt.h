#ifndef SYS_GDT_H_
#define SYS_GDT_H_

namespace sys {

// Install the system's global descriptor table.
void InstallGlobalDescriptorTable();

}

#endif  // SYS_GDT_H_
