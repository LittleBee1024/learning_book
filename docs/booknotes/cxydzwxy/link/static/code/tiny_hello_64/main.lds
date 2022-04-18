ENTRY(nomain)

SECTIONS
{
   . = 0x400000 + SIZEOF_HEADERS;
   tinytext : { *(.text) *(.data) *(.rodata) }
   /DISCARD/ : { *(.comment) }
}
