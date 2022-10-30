#include "timer.h"
#include "sys.h"
#include "proc.h"

static int timer_count = 0;

#define INTERVAL_MULTI 3
void timer_init()
{
    // each CPU has a separate source of timer interrupts.
    int id = r_mhartid();

    // ask the CLINT for a timer interrupt.
    *(reg_t*)CLINT_MTIMECMP(id) = *(reg_t*)CLINT_MTIME + interval * INTERVAL_MULTI;

    // set the machine-mode trap handler.
    w_mtvec((reg_t)sys_timer);

    // enable machine-mode interrupts.
    w_mstatus(r_mstatus() | MSTATUS_MIE);

    // enable machine-mode timer interrupts.
    w_mie(r_mie() | MIE_MTIE);
    lib_puts("timer init fin\n");
}

void preemptive();
reg_t timer_handler(reg_t epc, reg_t cause)
{
    lib_printf("epc %x\n", epc);
    reg_t return_pc = reinterpret_cast<reg_t>(&preemptive);
//    reg_t return_pc = epc;
    // disable machine-mode timer interrupts.
    w_mie(~((~r_mie()) | (1 << 7)));


    lib_printf("timer_handler: %d\n", ++timer_count);
    int id = r_mhartid();
    *(reg_t *)CLINT_MTIMECMP(id) = *(reg_t *)CLINT_MTIME + interval * INTERVAL_MULTI;
    // enable machine-mode timer interrupts.
    w_mie(r_mie() | MIE_MTIE);

    return return_pc;
}

void preemptive()
{
    // 除非提前保存pc
    //在这里开始回去调度，那么一会回来的也是这里，无法再回到user proc了
    delay(10000);
    lib_printf("pid: %d ", current_proc()->pid);
    current_proc()->time = timer_count;
    timer_count = 0;
    current_proc()->state = proc_sleeping;
    lib_puts("preemptive!\n");

    switch_to_sys_proc();
    lib_puts("end preemtive\n");

//    lib_printf("2 status:%d\n", sys_proc()[2].state);
//    run_proc(sys_proc());
}