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
extern kernel_main

_start:
    ; Hand over our newly allocated stack boundary pointer to the CPU stack register
    mov esp, stack_top

    ; Jump directly into our main freestanding C++ environment kernel loop
    call kernel_main

    ; Safety fallback: if kernel_main ever returns, freeze the processor execution
    cli
halt_loop:
    hlt
    jmp halt_loop

section .bss
align 16
stack_bottom:
    resb 16384 ; Allocate a 16KB uninitialized data area for our temporary system stack pointer
stack_top:
