#include "os.h"
#include "riscv.h"
#include "libc.h"
#include "timer.h"
#include "proc.h"
extern "C" {
void trap_vector();

void trap_init()
{
    // set the machine-mode trap handler.
    w_mtvec((reg_t)trap_vector);

    // enable machine-mode interrupts.
    w_mstatus(r_mstatus() | MSTATUS_MIE);
}

void preemptive(reg_t);

reg_t trap_handler(reg_t epc, reg_t cause) {
    reg_t ret_pc = epc;
    reg_t cause_code = cause & 0xfff;
    lib_printf("cause:%x\n", cause);
    if (cause & 0x80000000) {
        /* Asynchronous trap - interrupt */
        switch (cause_code) {
            case 3:
                lib_puts("software interruption!\n");
                break;
            case 7:
                lib_puts("timer interruption!\n");
                // disable machine-mode timer interrupts.
                w_mie(~((~r_mie()) | (1 << 7)));
                timer_handler();
                ret_pc = (reg_t) &preemptive;
                // enable machine-mode timer interrupts.
                w_mie(r_mie() | MIE_MTIE);
                break;
            case 11:
                lib_puts("external interruption!\n");
                break;
            default:
                lib_puts("unknown async exception!\n");
                break;
        }
    } else {
        /* Synchronous trap - exception */
        lib_puts("Sync exceptions!\n");
        while (1) {
            /* code */
        }
    }
    return ret_pc;
}
}

void preemptive(reg_t epc)
{
    lib_printf("epc:%x\n", epc);
    // 除非提前保存pc
    //在这里开始回去调度，那么一会回来的也是这里，无法再回到user proc了
    delay(10000);
    lib_printf("pid: %d ", current_proc()->pid);
//    current_proc()->time = timer_count;
//    timer_count = 0;
    current_proc()->state = proc_sleeping;
    lib_puts("preemptive!\n");

    switch_to_sys_proc();
    lib_puts("end preemtive\n");

    //    lib_printf("2 status:%d\n", sys_proc()[2].state);
    //    run_proc(sys_proc());
}