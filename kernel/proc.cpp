#include "proc.h"
#include "libc.h"

uint8_t task_stack[PROC_MAX_CNT][STACK_SIZE];

inline proc proc_list[PROC_MAX_CNT];
inline proc *curr_proc;
inline int proc_top;

proc *sys_proc() {
    return &proc_list[0];
}

void proc_foreach(void (p)(proc *)) {
    for (int i = 0; i < proc_top; ++i) {
        p(&proc_list[i]);
    }
}

// todo: s2 is a bad design
void user_proc() {
    auto user_proc_addr = curr_proc->ctxt.s2;
    reinterpret_cast<void(*)()>(user_proc_addr)();
    curr_proc->state = proc_used;
    run_proc(sys_proc());
}

proc* set_proc(proc* p, proc_ctxt ctxt, void (*proc_addr)(), proc_state state){
    p->pid = proc_top;
    p->ctxt = ctxt;
    p->proc_addr = proc_addr;
    p->ctxt.ra = reinterpret_cast<reg_t>(user_proc);
    p->ctxt.s2 = reinterpret_cast<reg_t>(proc_addr);
    p->ctxt.sp = reinterpret_cast<reg_t>(&task_stack[proc_top][STACK_SIZE-1]);
    p->state = state;
    return p;
}

proc *create_proc(void (*proc_addr)()) {
    auto *p = set_proc(&proc_list[proc_top], proc_ctxt(), proc_addr, proc_runnable);
    proc_top++;
    return p;
}

void run_proc(proc *p) {
    if(curr_proc->pid == p->pid && p != sys_proc()) {

    }
    lib_printf("switch proc from %d to %d\n", curr_proc->pid, p->pid);
//    lib_printf("from %x to %x\n", curr_proc->ctxt.ra, p->ctxt.ra);
    curr_proc->state = proc_used;
    p->state = proc_running;

    auto old_p = curr_proc;
    curr_proc = p;
    sys_switch(&old_p->ctxt, &p->ctxt);
    lib_printf("%d over state %s\n", p->pid, to_string(p->state));
}

void init_first_proc() {
    set_proc(&proc_list[0], proc_ctxt(), &main_proc, proc_running);
    curr_proc = proc_list;
    proc_top = 1;
}


proc *schedule() {
    for (int i = 0; i < proc_top; ++i) {
        if (proc_list[i].state == proc_runnable || proc_list[i].state == proc_sleeping) {
            lib_printf("schedule to proc%d\n", proc_list[i].pid);
            return &proc_list[i];
        }
    }
    ERROR("unreachable");
    while(1) {

    }
    return nullptr;
}

void main_proc() {
    while(1) {
        delay(10000);
        run_proc(schedule());
    }
}

proc *current_proc() {
    return curr_proc;
}

void print_proc(proc *p) {
    lib_printf("proc pic:%d status %s\n", p->pid, to_string(p->state));
}

void show_proc() {
    proc_foreach(print_proc);
}

const char* to_string(proc_state state) {
    switch (state) {
        case proc_unused:
            return "proc_unused";
        case proc_used:
            return "proc_used";
        case proc_sleeping:
            return "proc_sleeping";
        case proc_runnable:
            return "proc_runnable";
        case proc_running:
            return "proc_running";
        case proc_zombie:
            return "proc_zombie";
    }
}

void switch_to_sys_proc() {
    auto old_p = curr_proc;
    curr_proc = sys_proc();
    sys_switch(&old_p->ctxt, &sys_proc()->ctxt);
    lib_puts("switch_to_sys_proc end\n");
}
