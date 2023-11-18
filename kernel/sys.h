#pragma once

struct proc_ctxt;

extern "C" {
    void sys_switch(proc_ctxt*, proc_ctxt*);
    void sys_timer();
}
