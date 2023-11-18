#include "mm.h"
#include "spinlock.h"
#include "param.h"

struct file {

};

struct f_table {
    spinlock lock;
    file files[NFILE];
} ftable;

extern "C" void fileinit()
{
    initlock(&ftable.lock, "ftable");
}