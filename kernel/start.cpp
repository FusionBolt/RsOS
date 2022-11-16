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
    // enable supervisor mode
    auto status = r_mstatus();
    w_mstatus(status | SSTATUS_SPP);

    timer_init();

    // todo: what?? 如果不开这个会怎么样
    // configure Physical Memory Protection to give supervisor mode
    // access to all of physical memory.
    w_pmpaddr0(0x3fffffffffffffull);
    w_pmpcfg0(0xf);

    w_satp(0);

    w_mepc(reinterpret_cast<reg_t>(&os_main));
    lib_printf("end start\n");
    asm volatile("mret");
}
}