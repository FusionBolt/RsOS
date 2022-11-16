#include "libc.h"
#include "proc.h"
#include "../user/user_proc.h"
#include "../user/shell.h"
#include "mm.h"

extern "C" {
    void trap_init();
    void plic_init();

    int os_main()
    {
        lib_puts("os start\n");
        lib_puts("kinit\n");
        kinit();
        lib_puts("vminit\n");
        vminit();
        while(1) {
            delay(10000);
            lib_printf("time: %d\n", *reinterpret_cast<uint64_t*>(CLINT_MTIME));
        }
//        plic_init();
//        init_first_proc();
//        create_proc(&shell);
//        create_proc(&user_first_proc);
//        create_proc(&user_while_proc);
//        create_proc(&user_last_proc);
//        show_proc();
//        main_proc();
        return 0;
    }
}