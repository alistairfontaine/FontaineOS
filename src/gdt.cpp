#include "gdt.h"

// Allocate our flat memory array matrix of exactly 3 descriptors
struct gdt_entry gdt[3];
struct gdt_ptr   gp;

/*
   Internal utility to break apart 32-bit memory addresses and sizes
   and pack them into the specific bitfields the CPU hardware demands.
*/
void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[num].base_low    = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high   = (base >> 24) & 0xFF;

    gdt[num].limit_low   = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F);

    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access      = access;
}

/*
   The main initialization function called by our core kernel boot loop.
*/
void init_gdt() {
    // 1. Setup our GDT descriptor tracking pointer structure block
    gp.limit = (sizeof(struct gdt_entry) * 3) - 1;
    gp.base  = (uint32_t)&gdt;

    // 2. Gate 0: The Mandatory Intel Null Descriptor (Must be completely clear)
    gdt_set_gate(0, 0, 0, 0, 0);

    // 3. Gate 1: Kernel Code Segment (Base 0, Limit 4GB, Granularity: 4KB pages, 32-bit protected mode)
    // Access 0x9A = Present, Ring 0 (Kernel), Executable, Readable
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    // 4. Gate 2: Kernel Data Segment (Base 0, Limit 4GB, Granularity: 4KB pages, 32-bit protected mode)
    // Access 0x92 = Present, Ring 0 (Kernel), Read/Write, Writable
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    /*
       5. Inform the CPU register hardware where our table sits in RAM.
       We use inline assembly here because standard C++ cannot touch physical registers.
       - 'lgdt' loads our pointer mapping block.
       - The long jump 'ljmp' forces the CPU to instantly drop its old cache and link
         its internal Code Segment register (CS) to our new Gate 1 index (0x08 offset).
    */
    asm volatile(
        "lgdt (%0)\n\t"
        "ljmp $0x08, $.reload_segments\n\t"
        ".reload_segments:\n\t"
        "movw $0x10, %%ax\n\t"  // 0x10 is our Gate 2 Data Segment offset address index
        "movw %%ax, %%ds\n\t"
        "movw %%ax, %%es\n\t"
        "movw %%ax, %%fs\n\t"
        "movw %%ax, %%gs\n\t"
        "movw %%ax, %%ss\n\t"
        : : "r" (&gp) : "ax"
    );
}
