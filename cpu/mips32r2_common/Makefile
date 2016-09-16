ifeq ("$(OS)","Windows_NT") 
MIPS_ELF_ROOT=C:\cross-mips-mti-elf
else
MIPS_ELF_ROOT=/opt/imgtec/Toolchains/mips-mti-elf/2016.05-03
endif


MODULE = cpu

DIRS = periph

ABI=32
MEMORY_BASE=0x80000000
MEMORY_SIZE=1M
APP_START=0x80002000

include $(MIPS_ELF_ROOT)/share/mips/rules/mipshal.mk
include $(RIOTBASE)/Makefile.base
