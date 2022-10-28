#include "libc.h"
#include "proc.h"
#include "../user/user_proc.h"

extern "C" {
    int os_main()
    {
        lib_puts("os start\n");
        init_first_proc();
        auto *p = create_proc(&user_proc0);
        create_proc(&user_proc1);
        wait_proc();

        return 0;
    }
}