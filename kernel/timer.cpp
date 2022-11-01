#include "timer.h"
#include "sys.h"
#include "proc.h"
#include "riscv.h"

static int timer_count = 0;

#define INTERVAL_MULTI 3

// a scratch area per CPU for machine-mode timer interrupts.
reg_t timer_scratch[NCPU][5];

void timer_init()
{
    // each CPU has a separate source of timer interrupts.
    int id = r_mhartid();

    // ask the CLINT for a timer interrupt.
    *(reg_t*)CLINT_MTIMECMP(id) = *(reg_t*)CLINT_MTIME + interval * INTERVAL_MULTI;


    // prepare information in scratch[] for timervec.
    // scratch[0..2] : space for timervec to save registers.
    // scratch[3] : address of CLINT MTIMECMP register.
    // scratch[4] : desired interval (in cycles) between timer interrupts.
    reg_t *scratch = &timer_scratch[id][0];
    scratch[3] = CLINT_MTIMECMP(id);
    scratch[4] = interval;
    w_mscratch((reg_t)scratch);

    // enable machine-mode timer interrupts.
    w_mie(r_mie() | MIE_MTIE);
    lib_puts("timer init fin\n");
}

void timer_handler()
{
    lib_printf("timer_handler: %d\n", ++timer_count);
    int id = r_mhartid();
    *(reg_t *)CLINT_MTIMECMP(id) = *(reg_t *)CLINT_MTIME + interval * INTERVAL_MULTI;
}