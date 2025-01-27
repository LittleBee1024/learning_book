ENTRY(nomain)

SECTIONS
{
   . = 0x0804800 + SIZEOF_HEADERS;
   tinytext : { *(.text) *(.data) *(.rodata) }
   /DISCARD/ : { *(.comment) }
}
