#include "../kernel/libc.h"
#include "../kernel/riscv.h"

const char* get_line();

char buffer[1024];

void shell() {
    // set S Previous Privilege mode to User.
    unsigned long x = r_sstatus();
    x &= ~SSTATUS_SPP; // clear SPP to 0 for user mode
    x |= SSTATUS_SPIE; // enable interrupts in user mode
    w_sstatus(x);

    while(true) {
        lib_printf("prompt>");
        auto line = lib_gets(buffer);
        lib_printf("%s\n", line);
    }
}