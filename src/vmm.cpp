#include "vmm.h"

/*
   Allocate our two-tiered translation structures.
   We align these arrays to 4096-byte (4KB) boundaries using compiler attributes
   because the x86 CPU hardware requires page tables to sit flat on clean page lines.
*/
uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t first_page_table[1024] __attribute__((aligned(4096)));

void init_vmm() {
    // 1. Initialize all Page Directory lines as non-present (clear all entries to 0)
    for (int i = 0; i < 1024; i++) {
        /*
           Setting attribute flag 0x02 marks the entry as Read-Write but Not-Present.
           The CPU will trigger a Page Fault exception if anyone tries to touch it.
        */
        page_directory[i] = 0x02;
    }

    /*
       2. Populate our first Page Table.
       We loop through all 1,024 entries to Identity Map the first 4 Megabytes of RAM.
       (1024 pages * 4096 bytes per page = 4,194,304 bytes / 4MB)
    */
    for (uint32_t i = 0; i < 1024; i++) {
        uint32_t physical_address = i * 4096;

        /*
           0x03 marks the page entry as Present, Read-Write, and Supervisor (Kernel-only).
           We link the raw physical base address with these attribute flags.
        */
        first_page_table[i] = physical_address | 0x03;
    }

    /*
       3. Link our First Page Table into the First Slot of the Page Directory.
       0x03 means the page table itself is Present and writable by the kernel.
    */
    page_directory[0] = ((uint32_t)first_page_table) | 0x03;

    /*
       4. Arm the CPU Hardware Registers via Inline Assembly.
       - We move the physical memory location of our page directory array into CR3.
       - We read the CR0 configuration register, flip the highest bit (Bit 31 - Paging Enable),
         and write it back to instantly kick the hardware processor into virtual mode.
    */
    asm volatile(
        "mov %0, %%cr3\n\t"        // Load Page Directory pointer address into CR3
        "mov %%cr0, %%eax\n\t"     // Read current CR0 configuration state flags
        "or $0x80000000, %%eax\n\t" // Flip the bit 31 (PG flag) high to enable hardware paging matrix
        "mov %%eax, %%cr0\n\t"     // Flush the new configuration settings back to the processor execution unit
        : : "r"(page_directory) : "eax"
    );
}
