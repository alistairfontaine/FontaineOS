ENTRY(_start)

SECTIONS {
    /* Conventional safe location to boot our kernel binary into RAM */
    . = 1M;

    .text : {
        *(.multiboot)
        *(.text)
    }
    .rodata : { *(.rodata) }
    .data   : { *(.data) }
    .bss    : { *(.bss) }
}
