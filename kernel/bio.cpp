#include "spinlock.h"
#include "os.h"
#include "fs.h"
#include "param.h"
#include "mm.h"

struct buf {
    int valid;   // has data been read from disk?
    int disk;    // does disk "own" buf?
    uint32_t dev;
    uint32_t blockno;
//    sleeplock lock;
    uint32_t refcnt;
    buf *prev; // LRU cache list
    buf *next;
    unsigned char data[BSIZE];
};

struct b_cache {
    struct spinlock lock;
    struct buf buf[NBUF];

    // Linked list of all buffers, through prev/next.
    // Sorted by how recently the buffer was used.
    // head.next is most recent, head.prev is least.
    struct buf head;
};

extern "C" void binit()
{
    struct buf *b;
    b_cache *bcache = (b_cache*)kalloc();
    initlock(&bcache->lock, "bcache");
}