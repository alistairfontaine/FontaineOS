#include "gdt.h"
#include "idt.h"
#include "timer.h"
#include "pmm.h"
#include "vmm.h"
#include "heap.h"
#include "task.h"

// Shared global counters for our threads to update safely
uint32_t count_alpha = 0;
uint32_t count_beta = 0;

/*
   Thread Task Alpha.
   This code runs on its own private stack and updates its state counter on row 3.
*/
void task_alpha_routine() {
    volatile char* video_memory = (volatile char*)0xB8000;

    while (true) {
        count_alpha = count_alpha + 1;
        char state_char = '0' + (count_alpha % 10);

        // Write safely to a single character slot on row 3 (offset index 320 + 104 bytes)
        video_memory[424] = state_char;
        video_memory[425] = 0x0A; // Light Green style

        /*
           Bulletproof, warning-free delay loop for modern C++20.
           We use a basic loop block with an empty assembly volatile spacer
           to stop the compiler from throwing a deprecated assignment warning.
        */
        for (uint32_t delay = 0; delay < 5000000; delay++) {
            asm volatile("");
        }
    }
}

/*
   Thread Task Beta.
   This code runs on its own private stack and updates its state counter on row 4.
*/
void task_beta_routine() {
    volatile char* video_memory = (volatile char*)0xB8000;

    while (true) {
        count_beta = count_beta + 1;
        char state_char = '0' + (count_beta % 10);

        // Write safely to a single character slot on row 4 (offset index 480 + 104 bytes)
        video_memory[584] = state_char;
        video_memory[585] = 0x0D; // Light Purple style

        /* Bulletproof warning-free delay loop */
        for (uint32_t delay = 0; delay < 5000000; delay++) {
            asm volatile("");
        }
    }
}

extern "C" void kernel_main() {
    /* Step 1: Initialize the Core System Engine Segments */
    init_gdt();
    init_idt();
    init_timer(100);
    init_pmm(64 * 1024 * 1024);
    init_vmm();
    init_heap(0x300000, 256);

    /* Step 2: Initialize the Multitasking Scheduler Layer */
    init_multitasking();

    /* Step 3: Spawn our dynamic parallel threads */
    create_thread(task_alpha_routine);
    create_thread(task_beta_routine);

    /* Render Baseline Screen Texts ONCE before interrupts fire to eliminate layout drift */
    volatile char* video_memory = (volatile char*)0xB8000;

    const char* msg_master = "FontaineOS Architecture Complete! Task Scheduler Loops Active.";
    const char* msg_alpha  = "[Task Alpha Running Concurrently] Cycle State Tick: ";
    const char* msg_beta   = "[Task Beta Running Concurrently] Cycle State Tick:  ";

    int i = 0;
    while (msg_master[i] != '\0') {
        video_memory[160 + (i * 2)] = msg_master[i];
        video_memory[160 + (i * 2) + 1] = 0x0E; // Bright Gold text style
        i++;
    }

    i = 0;
    while (msg_alpha[i] != '\0') {
        video_memory[320 + (i * 2)] = msg_alpha[i];
        video_memory[320 + (i * 2) + 1] = 0x0A; // Light Green style
        i++;
    }

    i = 0;
    while (msg_beta[i] != '\0') {
        video_memory[480 + (i * 2)] = msg_beta[i];
        video_memory[480 + (i * 2) + 1] = 0x0D; // Light Purple style
        i++;
    }

    /*
       Step 4: Enable Hardware Interrupts globally.
       The physical motherboard timer at 100Hz will now act as our primary
       multitasking engine, swapping stacks cleanly behind the scenes.
    */
    asm volatile("sti");

    /* Main Core Master Loop Timeline */
    while (true) {
        // Relax the main thread, letting the timer interrupt orchestrate the tasks
        asm volatile("hlt");
    }
}
