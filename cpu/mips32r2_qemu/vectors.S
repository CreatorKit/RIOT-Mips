#include <mips/m32c0.h>
#include <mips/regdef.h>
#include <mips/asm.h>

.set noreorder
.set noat

.org 0x180
.globl __irq_handler
__irq_handler:
    /* __context_save doesn't save the $31 */
    sw    ra, -4(sp)
    jal   __context_save
    nop
    /* save return address */
    mfc0  t0, C0_EPC
    sw    t0, 116(sp)

    /* TODO: have a separate irq context */
    
    /** jump to generic handler **/
    jal   generic_exception_handler
    nop

    /* TODO: check for sched_context_switch_request */

    /* restore original context */
    jal   __context_restore
    nop
    lw    ra, -12(sp)
    mtc0  ra, C0_EPC
    /* similarly we need to restore $31 separately */
    lw    ra, -4(sp)
    eret
