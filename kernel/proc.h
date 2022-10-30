#pragma once
#include "riscv.h"
#include "os.h"
#include "string.h"
#include "sys.h"
#include <stdint.h>

enum proc_state { proc_unused,
                  proc_used,
                  proc_sleeping,
                  proc_runnable,
                  proc_running,
                  proc_zombie };

struct proc {
    proc_ctxt ctxt;
    int pid;
    int time;
    void (*proc_addr)();
    proc_state state;
};

const char* to_string(proc_state state);

proc* set_proc(proc* p, proc_ctxt ctxt, void (*proc_addr)(), proc_state state);


proc *create_proc(void (*proc_addr)());

void run_proc(proc *p);

proc *sys_proc();

void main_proc();

void init_first_proc();

proc *schedule();

proc *current_proc();

void proc_foreach(void (p)(proc *));

void show_proc();

void switch_to_sys_proc();
