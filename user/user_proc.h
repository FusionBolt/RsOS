#pragma once
#include "../kernel/libc.h"
#include "../kernel/proc.h"
int user_proc0()
{
    lib_puts("proc0");
    run_proc(&proc_list[0]);
}

int user_proc1()
{
    lib_puts("proc1");
    run_proc(&proc_list[0]);
}