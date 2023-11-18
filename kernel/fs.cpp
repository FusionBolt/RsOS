#include "spinlock.h"
#include "sleeplock.h"
#include "param.h"
#include "mm.h"

struct inode {
    sleeplock lock;
};

struct itable {
    spinlock lock;
    inode inodelist[NINODE];
} table;

extern "C" void iinit()
{
    initlock(&table.lock, "itable");
    for (int i = 0; i < NINODE; ++i) {
        init_sleeplock(&table.inodelist[i].lock, "inode");
    }
}