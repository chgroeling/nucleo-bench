/* Linker script: STM32F446RE — 512K FLASH @ 0x08000000, 128K RAM @ 0x20000000. */

MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 512K
    RAM   (rwx): ORIGIN = 0x20000000, LENGTH = 128K
}

_estack = ORIGIN(RAM) + LENGTH(RAM);

/* Heap and stack reservation. The heap is only ever used if your algorithm
   allocates (malloc/free/new/delete); _sbrk hands out bytes from this region.
   These are link-time size guarantees — the linker errors if RAM can't hold
   .data + .bss + this heap + this stack. Tune to your algorithm's needs. */
_Min_Heap_Size  = 0x19000;  /* 100 KiB heap */
_Min_Stack_Size = 0x2800;   /* 10 KiB stack */

SECTIONS
{
    .isr_vector :
    {
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > FLASH

    .text :
    {
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } > FLASH

    .preinit_array :
    {
        PROVIDE_HIDDEN(__preinit_array_start = .);
        KEEP(*(.preinit_array*))
        PROVIDE_HIDDEN(__preinit_array_end = .);
    } > FLASH

    .init_array :
    {
        PROVIDE_HIDDEN(__init_array_start = .);
        KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN(__init_array_end = .);
    } > FLASH

    .fini_array :
    {
        PROVIDE_HIDDEN(__fini_array_start = .);
        KEEP(*(SORT_BY_INIT_PRIORITY(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN(__fini_array_end = .);
    } > FLASH

    _sidata = LOADADDR(.data);

    .data :
    {
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > RAM AT > FLASH

    .bss :
    {
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > RAM

    /* Heap grows up from _sheap (== _end) via _sbrk; the stack grows down from
       _estack. No section is emitted here — that keeps the heap/stack out of
       the reported .bss so the baseline size is unaffected. The ASSERT is a
       pure link-time check that RAM can hold .data + .bss plus the requested
       heap and stack; it costs zero bytes. */
    . = ALIGN(8);
    PROVIDE(_end = .);
    PROVIDE(end = .);
    _sheap = .;

    ASSERT(_sheap + _Min_Heap_Size + _Min_Stack_Size <= _estack,
           "RAM overflow: not enough room for heap + stack")
}
