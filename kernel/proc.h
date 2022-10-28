#pragma once
#include <stdint.h>
#include "string.h"
#include "defs.h"
#include "stack.h"

using reg_t = uint64_t;
struct proc_ctxt {
    reg_t ra;
    reg_t sp;
    reg_t s0;
    reg_t s1;
    reg_t s2;
    reg_t s3;
    reg_t s4;
    reg_t s5;
    reg_t s6;
    reg_t s7;
    reg_t s8;
    reg_t s9;
    reg_t s10;
    reg_t s11;
};

enum proc_state { proc_unused,
                  proc_used,
                  proc_sleeping,
                  proc_runnable,
                  proc_running,
                  proc_zombie };

struct proc {
    int pid;
    proc_ctxt ctxt;
    int (*proc_addr)();
    proc_state state;
};

inline proc proc_list[PROC_MAX_CNT];
inline proc *curr_proc;
inline int proc_top;

proc* set_proc(proc* p, proc_ctxt ctxt, int (*proc_addr)(), proc_state state){
    p->pid = proc_top;
    p->ctxt = ctxt;
    p->proc_addr = proc_addr;
    p->ctxt.ra = reinterpret_cast<reg_t>(proc_addr);
    p->ctxt.sp = reinterpret_cast<reg_t>(task_stack[proc_top]);
    p->state = state;
    return p;
}

proc *create_proc(int (*proc_addr)()) {
    auto *p = set_proc(&proc_list[proc_top], proc_ctxt(), proc_addr, proc_runnable);
    proc_top++;
    return p;
}

int run_proc(proc *p) {
    if(curr_proc->pid == p->pid) {

    }
//    lib_printf("switch proc from %d to %d\n", curr_proc->pid, p->pid);
//    lib_printf("from %x to %x\n", curr_proc->ctxt.ra, p->ctxt.ra);
    curr_proc->state = proc_used;
    p->state = proc_running;
    auto old_p = p;
    curr_proc = p;
    sys_switch(&old_p->ctxt, &p->ctxt);
}

int wait_proc();

void init_first_proc() {
    set_proc(&proc_list[0], proc_ctxt(), &wait_proc, proc_runnable);
    curr_proc = proc_list;
    proc_top = 1;
}

proc *schedule() {
//    lib_puts("start schedule\n");
    for (int i = 1; i < PROC_MAX_CNT; ++i) {
        if (proc_list[i].state == proc_runnable) {
            lib_printf("to proc%d\n", proc_list[i].pid);
            return &proc_list[i];
        }
    }
//    lib_puts("default");
    return &proc_list[0];
}

int wait_proc() {
    while(1) {
        delay(10);
//        lib_puts("wait\n");
        run_proc(schedule());
    }
}