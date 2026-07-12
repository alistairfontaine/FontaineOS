#ifndef IDT_H
#define IDT_H

#include <stdint.h>

/*
   This structure defines a single 8-byte IDT entry gate descriptor.
   We pack this exactly so the Intel processor registers can parse the bit offsets.
*/
struct idt_entry {
    uint16_t base_low;      // The lower 16 bits of the address pointer to our C++ handler function
    uint16_t sel;           // The target Kernel Code Segment selector index (which will be 0x08 from our GDT)
    uint8_t  always0;       // A reserved hardware byte that must always remain completely set to 0
    uint8_t  flags;         // Access flags determining gate type, privilege level, and presence state
    uint16_t base_high;     // The upper 16 bits of the address pointer to our C++ handler function
} __attribute__((packed));

/*
   This pointer structure tells the CPU hardware where our IDT array lives in RAM layout.
*/
struct idt_ptr {
    uint16_t limit;         // The total size of the IDT array matrix minus 1 byte
    uint32_t base;          // The linear physical memory address where our table array starts
} __attribute__((packed));

/*
   Exposing our primary engine functions to the rest of the kernel system layers.
*/
void init_idt();

#endif
