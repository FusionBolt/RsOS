# Context switch
#
#   void sys_switch(struct context *old, struct context *new);
#
# Save current registers in old. Load from new.

.macro reg_save base
        # save the registers.
        sd ra, 0(\base)
        sd sp, 8(\base)
        sd gp, 16(\base)
        sd tp, 24(\base)
        sd t0, 32(\base)
        sd t1, 40(\base)
        sd t2, 48(\base)
        sd s0, 56(\base)
        sd s1, 64(\base)
        sd a0, 72(\base)
        sd a1, 80(\base)
        sd a2, 88(\base)
        sd a3, 96(\base)
        sd a4, 104(\base)
        sd a5, 112(\base)
        sd a6, 120(\base)
        sd a7, 128(\base)
        sd s2, 136(\base)
        sd s3, 144(\base)
        sd s4, 152(\base)
        sd s5, 160(\base)
        sd s6, 168(\base)
        sd s7, 176(\base)
        sd s8, 184(\base)
        sd s9, 192(\base)
        sd s10, 200(\base)
        sd s11, 208(\base)
        sd t3, 216(\base)
        sd t4, 224(\base)
        sd t5, 232(\base)
        sd t6, 240(\base)
.endm

.macro reg_load base
        ld ra, 0(\base)
        ld sp, 8(\base)
        ld gp, 16(\base)
        # not this, in case we moved CPUs: ld tp, 12(\base)
        ld t0, 32(\base)
        ld t1, 40(\base)
        ld t2, 48(\base)
        ld s0, 56(\base)
        ld s1, 64(\base)
        ld a0, 72(\base)
        ld a1, 80(\base)
        ld a2, 88(\base)
        ld a3, 96(\base)
        ld a4, 104(\base)
        ld a5, 112(\base)
        ld a6, 120(\base)
        ld a7, 128(\base)
        ld s2, 136(\base)
        ld s3, 144(\base)
        ld s4, 152(\base)
        ld s5, 160(\base)
        ld s6, 168(\base)
        ld s7, 176(\base)
        ld s8, 184(\base)
        ld s9, 192(\base)
        ld s10, 200(\base)
        ld s11, 208(\base)
        ld t3, 216(\base)
        ld t4, 224(\base)
        ld t5, 232(\base)
        ld t6, 240(\base)
.endm
# ============ Macro END   ==================


.globl sys_switch
sys_switch:
        sd ra, 0(a0)
        sd sp, 8(a0)
        sd s0, 16(a0)
        sd s1, 24(a0)
        sd s2, 32(a0)
        sd s3, 40(a0)
        sd s4, 48(a0)
        sd s5, 56(a0)
        sd s6, 64(a0)
        sd s7, 72(a0)
        sd s8, 80(a0)
        sd s9, 88(a0)
        sd s10, 96(a0)
        sd s11, 104(a0)

        ld ra, 0(a1)
        ld sp, 8(a1)
        ld s0, 16(a1)
        ld s1, 24(a1)
        ld s2, 32(a1)
        ld s3, 40(a1)
        ld s4, 48(a1)
        ld s5, 56(a1)
        ld s6, 64(a1)
        ld s7, 72(a1)
        ld s8, 80(a1)
        ld s9, 88(a1)
        ld s10, 96(a1)
        ld s11, 104(a1)

        ret


.globl trap_vector
# the trap vector base address must always be aligned on a 4-byte boundary
.align 4
trap_vector:
	# save context(registers).
	csrrw	t6, mscratch, t6	# swap t6 and mscratch
    reg_save t6
	csrw	mscratch, t6
	# call the C trap handler in trap.c
	csrr	a0, mepc
	csrr	a1, mcause
	call	trap_handler

	# trap_handler will return the return address via a0.
	csrw	mepc, a0

	# load context(registers).
	csrr	t6, mscratch
	reg_load t6
	mret

.globl timervec
.align 4
timervec:
        # start.c has set up the memory that mscratch points to:
        # scratch[0,8,16] : register save area.
        # scratch[24] : address of CLINT's MTIMECMP register.
        # scratch[32] : desired interval between interrupts.

        csrrw a0, mscratch, a0
        sd a1, 0(a0)
        sd a2, 8(a0)
        sd a3, 16(a0)

        # schedule the next timer interrupt
        # by adding interval to mtimecmp.
        ld a1, 24(a0) # CLINT_MTIMECMP(hart)
        ld a2, 32(a0) # interval
        ld a3, 0(a1)
        add a3, a3, a2
        sd a3, 0(a1)

        # arrange for a supervisor software interrupt
        # after this handler returns.
        li a1, 2
        csrw sip, a1

        ld a3, 16(a0)
        ld a2, 8(a0)
        ld a1, 0(a0)
        csrrw a0, mscratch, a0

        mret
