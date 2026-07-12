#include "gdt.h"

/*
   We wrap our main entrypoint in extern "C".
   This stops the C++ compiler from mangling the function name.
*/
extern "C" void kernel_main() {
    /*
       Step 1: Initialize the Global Descriptor Table (GDT).
       This reconfigures the CPU's memory segment mapping rules on the fly.
    */
    init_gdt();

    /*
       0xB8000 is the hardcoded x86 physical memory address for the VGA text frame buffer.
    */
    volatile char* video_memory = (volatile char*)0xB8000;

    /* Updated test string message to confirm segment execution state */
    const char* message = "Welcome to FontaineOS! GDT Segments Flashed Safely.";

    int i = 0;
    while (message[i] != '\0') {
        /* Write the actual text character byte */
        video_memory[i * 2] = message[i];

        /*
           0x0A represents a brilliant Light Green text color on a Black background matrix.
           We change the color to green so you instantly know the GDT phase succeeded!
        */
        video_memory[i * 2 + 1] = 0x0A;

        i++;
    }

    /*
       Keep the kernel alive in an infinite loop.
    */
    while (true) {
        // CPU remains completely protected inside our flat segmented memory boundaries
    }
}
