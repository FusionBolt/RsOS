#pragma once

struct proc_ctxt;

extern "C" {
    void sys_switch(struct proc_ctxt*, struct proc_ctxt*);
    void sys_timer();
}
