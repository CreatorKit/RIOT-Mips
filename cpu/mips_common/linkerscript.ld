OUTPUT_ARCH(mips)

ENTRY(__reset)

_FLASH_START = 0xbfc00000;
_RAM_START = 0x80100000;
_RAM_SIZE = 128K;

SECTIONS
{
    . = _FLASH_START;
    .reset : {
           *cpu.a:startup.o(.text)
    }

    . = 0x80000000;
    .vectors : {
           *cpu.a:vectors.o(.text)
    }

    . = _RAM_START;
    .text : { *(.text .text*) }
    .data : { *(.data) }
    .bss : { *(.bss) }

    . = ALIGN(8);
    _gp = . + 32K;
    .sdata :
    {
        *(.sdata)
        *(.sdata.*)
        *(.sbss)
        *(.sbss.*)
         _sheap = .;
         _eheap = . + 0x1000;
    }

    . = _RAM_START + _RAM_SIZE - 8;
    . = ALIGN(8);
    _sp = .;
}
