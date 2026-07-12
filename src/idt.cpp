#include "idt.h"

// Allocate our system array matrix of exactly 256 interrupt lines
struct idt_entry idt[256];
struct idt_ptr   idp;

/*
   Internal utility to split our 32-bit function memory addresses
   and map them into the required 8-byte Intel hardware gate structures.
*/
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low  = (base & 0xFFFF);
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].sel       = sel;
    idt[num].always0   = 0;

    /*
       Flags 0x8E = Present, Ring 0 (Kernel Privilege Mode),
       32-bit Interrupt Gate descriptor format profile.
    */
    idt[num].flags     = flags;
}

/*
   Exposing an explicit assembly label hook. This matches the special low-level
   assembly wrapper we will create inside our boot sequence in the next phase.
*/
extern "C" void isr0_handler_stub();

/*
   Our direct C++ exception target routine. When an exception triggers,
   the CPU context loops straight here.
*/
extern "C" void divide_by_zero_handler() {
    volatile char* video_memory = (volatile char*)0xB8000;
    const char* error_msg = "PANIC: KERNEL TRAPPED A DIVISION BY ZERO EXCEPTION!";

    int i = 0;
    while (error_msg[i] != '\0') {
        video_memory[i * 2] = error_msg[i];

        /*
           0x4F represents a brilliant White text color mapped onto an
           Emergency Bright Red alert background matrix.
        */
        video_memory[i * 2 + 1] = 0x4F;
        i++;
    }

    // Halt the core execution tracking sequence completely for hardware system safety
    while (true) {
        asm volatile("cli; hlt");
    }
}

/*
   The primary initialization function called by our core kernel boot loop.
*/
void init_idt() {
    // 1. Setup our IDT descriptor tracking pointer structure block
    idp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idp.base  = (uint32_t)&idt;

    // 2. Clear out all 256 gates initially by cycling through the memory lines
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    /*
       3. Register Interrupt Line 0 (Division by Zero Exception)
       0x08 is our target GDT Kernel Code Segment offset index selector.
    */
    idt_set_gate(0, (uint32_t)isr0_handler_stub, 0x08, 0x8E);

    /*
       4. Inform the CPU register hardware where our table sits in RAM space.
       'lidt' loads our pointer mapping block, loading the switchboard on the fly.
    */
    asm volatile("lidt (%0)" : : "r" (&idp));
}
