#include "mm.h"
#include "spinlock.h"
#include "param.h"

struct file {

};

struct f_table {
    spinlock lock;
    file files[NFILE];
};

extern "C" void fileinit()
{
    f_table *ftable = (f_table*)kalloc();
    initlock(&ftable->lock, "ftable");
}