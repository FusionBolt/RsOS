#include "libc.h"
#include "proc.h"
#include "../user/user_proc.h"
#include "../user/shell.h"
#include "mm.h"

extern "C" {
    void trap_init();
    void plic_init();
    void virtio_disk_init();
    void binit();
    void iinit();
    void fileinit();
    void scheduler();
    void first_user_proc_init();

    int os_main()
    {
        if(cpuid() != 0)
        {
            return 0;
        }
        lib_puts("os start\n");
        lib_puts("kinit\n");
        kinit();
        lib_puts("vminit\n");
        vminit();
        lib_puts("trap init\n");
        trap_init();
        plic_init();
        binit();
        iinit();
        fileinit();
        virtio_disk_init();
        lib_puts("init finish\n");
        first_user_proc_init();
        scheduler();

//        show_proc();
//        main_proc();
        return 0;
    }
}