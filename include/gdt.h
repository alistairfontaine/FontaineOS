#ifndef GDT_H
#define GDT_H

#include <stdint.h>

/*
   This structure defines a single 8-byte GDT entry descriptor.
   We use __attribute__((packed)) to prevent the C++ compiler from
   adding secret padding bytes, forcing it to match the exact hardware layout required by the Intel CPU registers.
*/
struct gdt_entry {
    uint16_t limit_low;     // The lower 16 bits of the memory segment limit size
    uint16_t base_low;      // The lower 16 bits of the memory segment base address
    uint8_t  base_middle;   // The next 8 bits of the memory segment base address
    uint8_t  access;        // Access flags determining segment privileges (Code/Data/Kernel/User)
    uint8_t  granularity;   // Size multiplier limits and operational mode bit flags
    uint8_t  base_high;     // The final 8 bits of the memory segment base address
} __attribute__((packed));

/*
   This special pointer structure tells the CPU hardware where our GDT array lives
   and exactly how large it is.
*/
struct gdt_ptr {
    uint16_t limit;         // The total size of the GDT array minus 1 byte
    uint32_t base;          // The linear physical memory address where the array starts
} __attribute__((packed));

/*
   Exposing our primary engine functions to the rest of the kernel system layers.
*/
void init_gdt();

#endif
