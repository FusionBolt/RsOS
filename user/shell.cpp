#include "../kernel/libc.h"
#include "../kernel/riscv.h"

const char* get_line();

char buffer[1024];

void shell() {
    while(true) {
        lib_printf("prompt>");
        auto line = lib_gets(buffer);
        lib_printf("%s\n", line);
    }
}