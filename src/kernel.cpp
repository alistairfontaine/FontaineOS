#include "gdt.h"
#include "idt.h"

extern "C" void kernel_main() {
    /* Step 1: Initialize the Global Descriptor Table (GDT) */
    init_gdt();

    /* Step 2: Initialize the Interrupt Descriptor Table (IDT) */
    init_idt();

    /*
       Step 3: Force a deliberate division-by-zero exception to test our IDT.
       We mark these variables 'volatile' so the C++ compiler's optimization layer
       doesn't calculate the result at compile-time. We force the raw laptop CPU
       hardware mathematical execution pipelines to evaluate this equation at runtime.
    */
    volatile int num = 10;
    volatile int den = 0;
    volatile int crash = num / den; // The CPU will instantly intercept this math error here!

    /* This line should NEVER be reached if our IDT works perfectly */
    volatile char* video_memory = (volatile char*)0xB8000;
    const char* message = "If you see this, the trap failed!";
    int i = 0;
    while (message[i] != '\0') {
        video_memory[i * 2] = message[i];
        video_memory[i * 2 + 1] = 0x0A;
        i++;
    }

    while (true) {}
}
