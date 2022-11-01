#pragma once
#include "os.h"
#include "libc.h"
#include "timer.h"

#define interval 10000000 // cycles; about 1 second in qemu.

void timer_init();

extern "C" {
    void timer_handler();
}
