#include "mm.h"
#include "proc.h"
#include "elf.h"

char *load_file(char *path) {

}

int get_fd(char *path) {

}

// todo: argc???
void exec(char *path, char *argv) {
    // 1. load path
    // 2. read elf info(entry)
    // 3. exec

    char* file = load_file(path);
    auto elf = (Elf64_Ehdr*)file;
    if(elf->e_ident != ELF_MAGIC) {
        panic("error elf ident");
    }
    if(elf->e_machine != "rv64") {

    }
    auto proc = proc_alloc();
    proc->ctxt.s2 = elf->e_entry;
    proc->state = proc_running;
    proc->fds[0] = get_fd(path);

    auto current_proc = get_current_proc();
    sys_switch(&current_proc->ctxt, &proc->ctxt);
    // load section
}

uint64_t sys_exec() {
    // get argument
    lib_printf("sys_exec\n");
    char *path, *argv;
    exec(path, argv);
    return 0;
}