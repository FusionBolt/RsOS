#pragma once
#include "libc.h"
#define STACK_SIZE 1024
#define PROC_MAX_CNT 10
#define ERROR(str) lib_printf("error: %s", str)
#include "riscv.h"

inline void panic(const char* str) {
    ERROR(str);
    while(1) {

    }
}