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
    void shell_init();
    void scheduler();

    int os_main()
    {
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
        shell_init();
        scheduler();

//        show_proc();
//        main_proc();
        return 0;
    }
}