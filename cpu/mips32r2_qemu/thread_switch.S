.set noreorder
.text
.globl __thread_start
.ent __thread_start
__thread_start:
    jal   __context_restore
    nop
    /* restore $31 */
    lw   $ra, -4($sp)
    /* load the return address */
    lw   $k0, -12($sp)
    jr   $k0
    ei
.end __thread_start
/**
__thread_switch is called from thread_yield().
Return register contains the last address. Save it
as it will be used to restore PC.
*/
.globl __thread_switch
.ent __thread_switch
__thread_switch:
    di
    /* save $31 */
    sw   $ra, -4($sp)
    /* actual return address. Here it is the same as $31 */
    sw   $ra, -12($sp)
    jal  __context_save
    nop
    jal  sched_run
    nop
    j    __thread_start
    nop
    /* UNREACHABLE */
    /* FIXME: ugly hack to force linker garbage collector to keep the
       __irq_handler (until I figure out how to properly use KEEP
       directive...). */
    lw $t0, __irq_handler
    nop
.end __thread_switch

.set noat
.globl __context_save
.ent __context_save
__context_save:
    /* allocate space on the stack for the registers */
    addiu   $sp, $sp, -128
    /* FIXME: store properly CP0.Status and CP0.Cause */

    /* Clear CP0.Status EXL in case the context is saved on IRQ entry */
    mfc0    $k0, $12, 0
    andi    $k1, $k0, 2
    xor     $k0, $k0, $k1

    mfc0    $k1, $13, 0

    sw      $1, 4($sp)
    sw      $2, 8($sp)
    sw      $3, 12($sp)
    sw      $4, 16($sp)
    sw      $5, 20($sp)
    sw      $6, 24($sp)
    sw      $7, 28($sp)
    sw      $8, 32($sp)
    sw      $9, 36($sp)
    sw      $10, 40($sp)
    sw      $11, 44($sp)
    sw      $12, 48($sp)
    sw      $13, 52($sp)
    sw      $14, 56($sp)
    sw      $15, 60($sp)
    sw      $16, 64($sp)
    sw      $17, 68($sp)
    sw      $18, 72($sp)
    sw      $19, 76($sp)
    sw      $20, 80($sp)
    sw      $21, 84($sp)
    sw      $22, 88($sp)
    sw      $23, 92($sp)
    sw      $24, 96($sp)
    sw      $25, 100($sp)
    sw      $26, 104($sp)
    sw      $27, 108($sp)
    sw      $28, 112($sp)
    /* $29 (sp) is saved in thread info. Therefore use that space for storing
       the return address (which will be different than $31 if we ended up
       here due to interrupt). */
    sw      $30, 120($sp)
    /* $31 must be saved outside of __context_save */

    lw      $24, sched_active_thread
    sw      $sp, 0($24)
    
    jr        $31
    nop
.end __context_save

.set noat    
.globl __context_restore
.ent __context_restore
__context_restore:
    /* restore sp */
    lw      $24, sched_active_thread
    lw      $sp, 0($24)

    /* restore remaining registers */
    lw      $1, 4($sp)
    lw      $2, 8($sp)
    lw      $3, 12($sp)
    lw      $4, 16($sp)
    lw      $5, 20($sp)
    lw      $6, 24($sp)
    lw      $7, 28($sp)
    lw      $8, 32($sp)
    lw      $9, 36($sp)
    lw      $10, 40($sp)
    lw      $11, 44($sp)
    lw      $12, 48($sp)
    lw      $13, 52($sp)
    lw      $14, 56($sp)
    lw      $15, 60($sp)
    lw      $16, 64($sp)
    lw      $17, 68($sp)
    lw      $18, 72($sp)
    lw      $19, 76($sp)
    lw      $20, 80($sp)
    lw      $21, 84($sp)
    lw      $22, 88($sp)
    lw      $23, 92($sp)
    lw      $24, 96($sp)
    lw      $25, 100($sp)
    lw      $26, 104($sp)
    lw      $27, 108($sp)
    lw      $28, 112($sp)
    /* $29 */
    lw      $30, 120($sp)
    /* $31 must be restored outside of __context_restore */

    mtc0  $k0, $12, 0
    mtc0  $k1, $13, 0
    addiu $sp, $sp, 128
    jr    $31
    nop
.end __context_restore
