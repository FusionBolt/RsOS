#pragma once
#include "../kernel/libc.h"
#include "../kernel/proc.h"
void user_first_proc()
{
    lib_puts("user first proc\n");
}

void user_last_proc()
{
    lib_puts("user last proc\n");
}

void user_while_proc()
{
    lib_puts("user while_proc\n");
    while(1) {
        delay(1000);
        lib_puts("in while\n");
    }
    lib_puts("end\n");
}