#include "task.h"
#include "heap.h"

// Tracking pointers mapping the active round-robin task looping rings
struct thread_control_block* running_task = nullptr;
struct thread_control_block* task_list_head = nullptr;

uint32_t next_thread_id = 1;

/*
   Initializes our multitasking environment by mapping our main
   kernel timeline execution branch into a permanent primary Task 0 node.
*/
void init_multitasking() {
    struct thread_control_block* kernel_task = (struct thread_control_block*)kmalloc(sizeof(struct thread_control_block));

    kernel_task->id = 0;
    kernel_task->esp = 0; // The main kernel already uses our primary 16KB system stack pool
    kernel_task->next = kernel_task; // Initially loop back into itself

    running_task = kernel_task;
    task_list_head = kernel_task;
}

/*
   Dynamically spawns a brand new parallel execution thread.
   Carves out a private stack workspace frame and links it into our running task chain ring.
*/
void create_thread(void (*thread_entry_function)()) {
    struct thread_control_block* new_thread = (struct thread_control_block*)kmalloc(sizeof(struct thread_control_block));

    // Allocate an isolated 1024-byte private stack buffer area from our virtual heap pool
    uint32_t private_stack_buffer = (uint32_t)kmalloc(1024);
    uint32_t private_stack_top = private_stack_buffer + 1024; // Stacks grow downward in x86 memory map layouts

    new_thread->id = next_thread_id;
    next_thread_id = next_thread_id + 1;

    /*
       Simulate a fresh stack structure block layout.
       We place the execution function address location right at the very bottom
       of the private stack memory boundary line.
    */
    private_stack_top = private_stack_top - 4;
    *(uint32_t*)private_stack_top = (uint32_t)thread_entry_function;

    /*
       Simulate the register state structure tracking configuration matrix.
       We push space for the 7 general purpose register structures so they pop off cleanly.
    */
    private_stack_top = private_stack_top - 28; // 7 registers * 4 bytes each = 28 bytes frame displacement

    new_thread->esp = private_stack_top;

    // Link the new thread node securely into our global circular round-robin task list loop ring
    new_thread->next = task_list_head->next;
    task_list_head->next = new_thread;
}

/*
   The Cooperative Scheduler Context Switcher.
   Saves the active register traces on the current stack, swaps the stack pointer variables,
   and wakes up the next linked target thread line.
*/
void switch_task() {
    if (running_task == nullptr || running_task->next == running_task) return;

    struct thread_control_block* old_task = running_task;
    struct thread_control_block* next_task = running_task->next;

    running_task = next_task;

    /*
       The Context Switch inline assembly execution matrix.
       - 'pusha' saves the active general-purpose registers onto the current stack.
       - We save the current stack pointer value into our old task structure variable tracking index.
       - We overwrite the CPU's active ESP stack pointer register with the next task's saved value.
       - 'popa' pops the next thread's saved registers back into the active silicon execution pipelines.
    */
    asm volatile(
        "pusha\n\t"
        "mov %%esp, %0\n\t"    // Save current stack frame context address
        "mov %1, %%esp\n\t"    // Swap the hardware tracking register onto the new thread's stack pathway
        "popa\n\t"
        : "=m"(old_task->esp)
        : "r"(next_task->esp)
        : "memory"
    );
}
