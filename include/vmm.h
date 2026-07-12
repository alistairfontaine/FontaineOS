#ifndef VMM_H
#define VMM_H

#include <stdint.h>

/*
   Exposing our primary virtual memory management initialization
   routine to our core kernel entry loop.
*/
void init_vmm();

#endif
