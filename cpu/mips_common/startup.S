#include <mips/m32c0.h>
#include <mips/regdef.h>
#include <mips/asm.h>

.set noreorder
.set noat

LEAF(__reset)
    j __init
    nop
END(__reset)

.org 0x380
    ori   $25, $0, 1
    ori    $4, $0, 11
    sdbbp 1

.org 0x480
    ori   $25, $0, 1
    ori    $4, $0, 12
    sdbbp 1    

.org 0x500
LEAF(__init)    
    LA gp, _gp
    LA sp, _sp
    LA t0, mips_start
    jr t0
    nop
END(__init)
