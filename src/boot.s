; Define constants for the Multiboot header alignment matrix
MAGIC    equ 0x1BADB002
FLAGS    equ 0x00
CHECKSUM equ -(MAGIC + FLAGS)

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

section .text
global _start
global isr0_handler_stub

extern kernel_main
extern divide_by_zero_handler

_start:
    ; Hand over our newly allocated stack boundary pointer to the CPU stack register
    mov esp, stack_top

    ; Jump directly into our main freestanding C++ environment kernel loop
    call kernel_main

    ; Safety fallback loop
    cli
halt_loop:
    hlt
    jmp halt_loop

; This is our raw low-level hardware entry stub for Interrupt 0
isr0_handler_stub:
    pusha                    ; Push all general-purpose CPU registers onto the stack to save their state

    call divide_by_zero_handler ; Jump directly into our C++ error log function

    popa                     ; Restore all general-purpose CPU registers back to normal state
    iret                     ; Interrupt Return: pops the saved CPU flags and tracking registers back on the fly

section .bss
align 16
stack_bottom:
    resb 16384 ; Allocate a 16KB uninitialized data area for our temporary system stack pointer
stack_top:
