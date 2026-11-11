MEMORY
{
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 256K
}

ENTRY(_entry)

SECTIONS
{
    .text : {
        KEEP(*(.entry))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
    } > RAM

    .data : { *(.data*) . = ALIGN(4); } > RAM
    .bss  : { *(.bss*) *(COMMON) . = ALIGN(4); } > RAM

    /DISCARD/ : { *(.ARM.exidx*) *(.ARM.attributes) *(.comment) }
}
