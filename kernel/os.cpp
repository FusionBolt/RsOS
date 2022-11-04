#include "libc.h"
#include "proc.h"
#include "../user/user_proc.h"
#include "timer.h"
#include "mm.h"

extern "C" {
    void trap_init();

    int os_main()
    {
        lib_puts("os start\n");
        kinit();
        timer_init();
        trap_init();
        init_first_proc();
        create_proc(&user_first_proc);
        create_proc(&user_while_proc);
        create_proc(&user_last_proc);
        show_proc();
        main_proc();
        return 0;
    }
}