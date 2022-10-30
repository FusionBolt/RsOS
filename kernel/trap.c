#include "os.h"
extern void trap_vector();

void trap_init()
{
    // set the machine-mode trap handler.
    w_mtvec((reg_t)trap_vector);

    // enable machine-mode interrupts.
    w_mstatus(r_mstatus() | MSTATUS_MIE);
}
