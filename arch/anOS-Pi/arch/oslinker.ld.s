/*ENTRY(__entry)*/

INCLUDE linker.ld.s

SECTIONS {
	
    .init 0x80000 : { *(.init) }

	.serial 0x90000 : { *(.serial) }

    .text 0x91000 : { *(.text) }

	/* read-only data, const */
    .rodata : { *(.rodata .rodata.*) }

	/* loaded data, has value */
    .data : { *(.data .data.* .got.plt) }

	/* unloaded data, symbol */
    .bss (NOLOAD) : {
        . = ALIGN(16);
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
    }
    _end = .;

	/DISCARD/ : {
		*(*)
	}
}