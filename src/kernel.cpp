#include "gdt.h"
#include "idt.h"
#include "timer.h"
#include "pmm.h"
#include "vmm.h"

extern "C" void kernel_main() {
    /* Step 1: Initialize the Global Descriptor Table (GDT) */
    init_gdt();

    /* Step 2: Initialize the Interrupt Descriptor Table (IDT) */
    init_idt();

    /* Step 3: Initialize the Programmable Interval Timer (PIT) at 100Hz */
    init_timer(100);

    /* Step 4: Initialize the Physical Memory Manager (PMM) with 64MB space bounds */
    init_pmm(64 * 1024 * 1024);

    /*
       Step 5: Initialize the Virtual Memory Manager (VMM).
       This sets up our identity tables, maps the initial 4MB layer,
       flashes the CR3 register, and triggers hardware paging via CR0 control.
    */
    init_vmm();

    /* Step 6: Enable Hardware Interrupts globally */
    asm volatile("sti");

    /* Print our active operational verification trace message */
    volatile char* video_memory = (volatile char*)0xB8000;
    const char* message = "FontaineOS Architecture Stable! Virtual Memory Paging Enabled.";

    int i = 0;
    while (message[i] != '\0') {
        video_memory[i * 2] = message[i];

        /*
           0x0B represents a brilliant Light Cyan / Cyan color on a Black background.
           We change the screen to light cyan to verify that the CPU safely transitioned
           from physical routing directly into active virtual address translation mode!
        */
        video_memory[i * 2 + 1] = 0x0B;
        i++;
    }

    while (true) {
        // CPU spins safely here, executing under complete virtual page layout protection
    }
}
