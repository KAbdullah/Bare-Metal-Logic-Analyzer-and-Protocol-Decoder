ENTRY(Reset_Handler)

MEMORY {
  FLASH(rx): ORIGIN = 0x08000000, LENGTH = 512K
  SRAM1(rwx): ORIGIN = 0x20000000, LENGTH = 128K
  SRAM2(rwx): ORIGIN = 0x2001C000, LENGTH = 16K
}

_estack = ORIGIN(SRAM1) + LENGTH(SRAM1);
_Min_Heap_Size = 0x400;
_Min_Stack_Size = 0x400;


SECTIONS 
{
  /*Iterrupt service routine vector table, also has the Reset_Handler function for intially moving*/ 
  .isr_vector : {
    KEEP(*(.isr_vector));
    . = ALIGN(4);
  } > FLASH

/*the programs code that are instructions*/
  .text : {
    . = ALIGN(4);
    _stext = .;
    PROVIDE(stext = .);
    *(.text)
    *(.text*)
    *(.glue_7)
    *(.glue_7t)
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))
    . = ALIGN(4);
    _etext = .;
    PROVIDE(etext = .);
  } > FLASH

/*Read-only data that can't be altered again like strings and consts*/
  .rodata : {
    . = ALIGN(4);
    _srodata = .;
    PROVIDE(srodata = .);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
    _erodata = .;
    PROVIDE(erodata = .);
  } > FLASH

/*read-write data that can be altered during run-time, so regular global and static variables*/
  .data : {
    . = ALIGN(4);
    _sdata = .;
    PROVIDE(sdata = .);
    *(.data)
    *(.data*)
    *(.RamFunc) 
    *(.RamFunc*)
    . = ALIGN(4);
    _edata = .;
    PROVIDE(edata = .);
  } > SRAM1 AT> FLASH

/*Reserved for uninitialized data*/
  .bss : {
    . = ALIGN(4);
    _sbss = .;
    PROVIDE(sbss = .);
    *(.bss)
    *(.bss*)
    *(COMMON)
    _ebss = .;
    PROVIDE(ebss = .);
    . = ALIGN(4);
  } > SRAM1

  .user_heap_stack : {
    . = ALIGN(4);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(4);
  } > SRAM1
}