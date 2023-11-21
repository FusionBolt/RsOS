#include "proc.h"
#include "libc.h"
#include "param.h"

cpu cpus[NCPU];
proc procs[NPROC];
uint8_t task_stack[NCPU][PROC_MAX_CNT][STACK_SIZE];
proc *current_proc;
cpu* get_cpu(int cpu_id)
{
    return &cpus[cpu_id];
}

cpu* get_current_cpu()
{
    return get_cpu(cpuid());
}

proc *get_current_proc()
{
    return get_current_cpu()->curr_proc;
}

int pid = 0;
int pic_alloc()
{
    pid++;
    return pid;
}

extern "C" void scheduler()
{
    auto cpu = get_current_cpu();
    while (true)
    {
        for (int i = 0; i < NPROC; ++i) {
            if(procs[i].state == proc_runnable) {
                lib_printf("switch to proc: %d", i);
                sys_switch(&cpu->ctxt, &procs[i].ctxt);
            }
        }
    }
}

page_table_t proc_pagetable(proc *p)
{
    page_table_t pagetable;
    pagetable = uvmcreate();
    return pagetable;
}

proc* proc_alloc()
{
    // 1. find first unused proc
    for (int i = 0; i < NPROC; ++i) {
        if(procs[i].state == proc_unused) {
            // 2. update basic info
            auto& proc = procs[i];
            proc.state = proc_runnable;
            proc.pid = pic_alloc();
            // todo: return address应该跳到哪里
            proc.ctxt.ra = 0;
            proc.trapframe = (trapframe_t *)kalloc();
            proc.pagetable = proc_pagetable(&proc);
            // s2 = proc_addr
            proc.ctxt.s2 = 0;
            // todo: stack size default value
            proc.ctxt.sp = proc.stack + STACK_SIZE;
            // alloc stack
            // todo: alloc和这样用预先分配的有什么区别？？
            // todo: 如果是预先分配的静态区域，那么是不是就没有写保护了
            // 或者说这块内存区域不在os的管理范围内？？
//            proc.ctxt.sp = reinterpret_cast<uint64_t>(&task_stack[cpuid()][i]);
            return &procs[i];
        }
    }

    panic("no empty proc");
}

// a user program that calls exec("/init")
// assembled from ../user/initcode.S
// od -t xC ../user/initcode
unsigned char initcode[] = {
        0x17, 0x05, 0x00, 0x00, 0x13, 0x05, 0x45, 0x02,
        0x97, 0x05, 0x00, 0x00, 0x93, 0x85, 0x35, 0x02,
        0x93, 0x08, 0x70, 0x00, 0x73, 0x00, 0x00, 0x00,
        0x93, 0x08, 0x20, 0x00, 0x73, 0x00, 0x00, 0x00,
        0xef, 0xf0, 0x9f, 0xff, 0x2f, 0x69, 0x6e, 0x69,
        0x74, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
};

extern "C" void first_user_proc_init()
{
    auto p = proc_alloc();
    // 分配了一块内存，映射到了虚拟地址0
    uvmfirst(p->pagetable, initcode, sizeof(initcode));
    p->memsize = PAGE_SIZE;
    // 这个pc是这个进程的虚拟内存中的地址，从这个进程的开始
    p->trapframe->epc = 0;
    // todo: 这个sp的位置
    p->trapframe->sp = PAGE_SIZE;
}

//uint8_t task_stack[PROC_MAX_CNT][STACK_SIZE];
//
//inline proc proc_list[PROC_MAX_CNT];
//inline proc *curr_proc;
//inline int proc_top;
//
//proc *sys_proc() {
//    return &proc_list[0];
//}
//
//void proc_foreach(void (p)(proc *)) {
//    for (int i = 0; i < proc_top; ++i) {
//        p(&proc_list[i]);
//    }
//}
//
//// todo: s2 is a bad design
//void user_proc() {
//    auto user_proc_addr = curr_proc->ctxt.s2;
//    reinterpret_cast<void(*)()>(user_proc_addr)();
//    curr_proc->state = proc_used;
//    run_proc(sys_proc());
//}
//
//proc* set_proc(proc* p, proc_ctxt ctxt, void (*proc_addr)(), proc_state state){
//    p->pid = proc_top;
//    p->ctxt = ctxt;
//    p->proc_addr = proc_addr;
//    p->ctxt.ra = reinterpret_cast<reg_t>(user_proc);
//    p->ctxt.s2 = reinterpret_cast<reg_t>(proc_addr);
//    p->ctxt.sp = reinterpret_cast<reg_t>(&task_stack[proc_top][STACK_SIZE-1]);
//    p->state = state;
//    return p;
//}
//
//proc *create_proc(void (*proc_addr)()) {
//    auto *p = set_proc(&proc_list[proc_top], proc_ctxt(), proc_addr, proc_runnable);
//    proc_top++;
//    return p;
//}
//
//void run_proc(proc *p) {
//    if(curr_proc->pid == p->pid && p != sys_proc()) {
//
//    }
//    lib_printf("switch proc from %d to %d\n", curr_proc->pid, p->pid);
////    lib_printf("from %x to %x\n", curr_proc->ctxt.ra, p->ctxt.ra);
//    curr_proc->state = proc_used;
//    p->state = proc_running;
//
//    auto old_p = curr_proc;
//    curr_proc = p;
//    sys_switch(&old_p->ctxt, &p->ctxt);
//    lib_printf("%d over state %s\n", p->pid, to_string(p->state));
//}
//
//void init_first_proc() {
//    set_proc(&proc_list[0], proc_ctxt(), &main_proc, proc_running);
//    curr_proc = proc_list;
//    proc_top = 1;
//}
//
//extern "C" void scheduler() {
//    // 找到对应的进程进行切换
//    while(true)
//    {
//        auto proc = schedule();
//        sys_switch(&curr_proc->ctxt, &proc->ctxt);
//    }
//}
//
//proc *schedule() {
//    for (int i = 0; i < proc_top; ++i) {
//        if (proc_list[i].state == proc_runnable || proc_list[i].state == proc_sleeping) {
//            lib_printf("proc %d is runnable\n", proc_list[i].pid);
//            return &proc_list[i];
//        }
//    }
//    ERROR("unreachable");
//    while(1) {
//
//    }
//    return nullptr;
//}
//
//void main_proc() {
//    while(1) {
//        delay(10000);
//        run_proc(schedule());
//    }
//}
//
//proc *current_proc() {
//    return curr_proc;
//}
//
//void print_proc(proc *p) {
//    lib_printf("proc pic:%d status %s\n", p->pid, to_string(p->state));
//}
//
//void show_proc() {
//    proc_foreach(print_proc);
//}
//
//const char* to_string(proc_state state) {
//    switch (state) {
//        case proc_unused:
//            return "proc_unused";
//        case proc_used:
//            return "proc_used";
//        case proc_sleeping:
//            return "proc_sleeping";
//        case proc_runnable:
//            return "proc_runnable";
//        case proc_running:
//            return "proc_running";
//        case proc_zombie:
//            return "proc_zombie";
//    }
//}
//
//void switch_to_sys_proc() {
//    auto old_p = curr_proc;
//    curr_proc = sys_proc();
//    sys_switch(&old_p->ctxt, &sys_proc()->ctxt);
//    lib_puts("switch_to_sys_proc end\n");
//}
