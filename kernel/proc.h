#pragma once
#include "riscv.h"
#include "os.h"
#include "string.h"
#include "sys.h"
#include <stdint.h>
#include "mm.h"
#include "param.h"

// used for kernel context switches
struct proc_ctxt {
    reg_t ra; // return address
    reg_t sp;

    // callee-saved
    reg_t s0;
    reg_t s1;
    reg_t s2; // proc address
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

// per-process data for the trap handling code in trampoline.S.
// sits in a page by itself just under the trampoline page in the
// user page table. not specially mapped in the kernel page table.
// uservec in trampoline.S saves user registers in the trapframe,
// then initializes registers from the trapframe's
// kernel_sp, kernel_hartid, kernel_satp, and jumps to kernel_trap.
// usertrapret() and userret in trampoline.S set up
// the trapframe's kernel_*, restore user registers from the
// trapframe, switch to the user page table, and enter user space.
// the trapframe includes callee-saved user registers like s0-s11 because the
// return-to-user path via usertrapret() doesn't return through
// the entire kernel call stack.
struct trapframe_t {
    /*   0 */ uint64_t kernel_satp;   // kernel page table
    /*   8 */ uint64_t kernel_sp;     // top of process's kernel stack
    /*  16 */ uint64_t kernel_trap;   // usertrap()
    /*  24 */ uint64_t epc;           // saved user program counter
    /*  32 */ uint64_t kernel_hartid; // saved kernel tp
    /*  40 */ uint64_t ra;
    /*  48 */ uint64_t sp;
    /*  56 */ uint64_t gp;
    /*  64 */ uint64_t tp;
    /*  72 */ uint64_t t0;
    /*  80 */ uint64_t t1;
    /*  88 */ uint64_t t2;
    /*  96 */ uint64_t s0;
    /* 104 */ uint64_t s1;
    /* 112 */ uint64_t a0;
    /* 120 */ uint64_t a1;
    /* 128 */ uint64_t a2;
    /* 136 */ uint64_t a3;
    /* 144 */ uint64_t a4;
    /* 152 */ uint64_t a5;
    /* 160 */ uint64_t a6;
    /* 168 */ uint64_t a7;
    /* 176 */ uint64_t s2;
    /* 184 */ uint64_t s3;
    /* 192 */ uint64_t s4;
    /* 200 */ uint64_t s5;
    /* 208 */ uint64_t s6;
    /* 216 */ uint64_t s7;
    /* 224 */ uint64_t s8;
    /* 232 */ uint64_t s9;
    /* 240 */ uint64_t s10;
    /* 248 */ uint64_t s11;
    /* 256 */ uint64_t t3;
    /* 264 */ uint64_t t4;
    /* 272 */ uint64_t t5;
    /* 280 */ uint64_t t6;
};

enum proc_state { proc_unused,
                  proc_used,
                  proc_sleeping,
                  proc_runnable,
                  proc_running,
                  proc_zombie };

struct proc {
    proc_ctxt ctxt;
    page_table_t pagetable;
    trapframe_t *trapframe;

    uint64_t stack; // stack addr
    int memsize;
    int pid;
    int time;
//    void (*proc_addr)();
    proc_state state;
    int fds[NOFILE];
};

struct cpu {
    proc* curr_proc;
    proc_ctxt ctxt;
};

proc *proc_alloc();

proc *get_current_proc();
//
//const char* to_string(proc_state state);
//
//proc* set_proc(proc* p, proc_ctxt ctxt, void (*proc_addr)(), proc_state state);
//
//
//proc *create_proc(void (*proc_addr)());
//
//void run_proc(proc *p);
//
//proc *sys_proc();
//
//void main_proc();
//
//void init_first_proc();
//
//proc *schedule();
//
//proc *current_proc();
//
//void proc_foreach(void (p)(proc *));
//
//void show_proc();
//
//void switch_to_sys_proc();
