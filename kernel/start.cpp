#include "riscv.h"
#include "os.h"
#include "memlayout.h"
#include "mm.h"

extern "C" {
extern int os_main();
extern char timervec[];
int os_main();

#define NCPU          8  // maximum number of CPUs
// a scratch area per CPU for machine-mode timer interrupts.
uint64_t timer_scratch[NCPU][5];

#define U64_TO_PTR(_u64v) reinterpret_cast<uint64_t *>(_u64v)
void timer_init() {
    auto id = cpuid();

    long interval = 1000000; // cycles; about 1/10th second in qemu.
    *U64_TO_PTR(CLINT_MTIMECMP(id)) = *U64_TO_PTR(CLINT_MTIME) + interval;

    // prepare information in scratch[] for timervec.
    // scratch[0..2] : space for timervec to save registers.
    // scratch[3] : address of CLINT MTIMECMP register.
    // scratch[4] : desired interval (in cycles) between timer interrupts.
    uint64_t *scratch = &timer_scratch[id][0];
    scratch[3] = CLINT_MTIMECMP(id);
    scratch[4] = interval;
    w_mscratch((uint64_t)scratch);

    w_mtvec(reinterpret_cast<reg_t>(timervec));
    w_mstatus(r_mstatus() | MSTATUS_MIE);
    // enable timer
    w_mie(r_mie() | MIE_MTIE);
}

void start() {
    // set M Previous Privilege mode to Supervisor, for mret.
    unsigned long x = r_mstatus();
    x &= ~MSTATUS_MPP_MASK;
    x |= MSTATUS_MPP_S;
    w_mstatus(x);

    w_satp(0);

    // delegate all interrupts and exceptions to supervisor mode.
    w_medeleg(0xffff);
    w_mideleg(0xffff);
    w_sie(r_sie() | SIE_SEIE | SIE_STIE | SIE_SSIE);

    // todo: what?? 如果不开这个会怎么样
    // configure Physical Memory Protection to give supervisor mode
    // access to all of physical memory.
    w_pmpaddr0(0x3fffffffffffffull);
    w_pmpcfg0(0xf);

    timer_init();

    w_mepc(reinterpret_cast<reg_t>(&os_main));
    lib_printf("end start\n");
    asm volatile("mret");
}
}