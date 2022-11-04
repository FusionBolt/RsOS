#include "mm.h"
#include "memlayout.h"
#include <stdint.h>
#include "string.h"
extern char end[];

struct free_page {
    free_page *next;
};

using pte_t = uint64_t;
using page_table_t = uint64_t;

page_table_t kernel_page_table;
free_page *free_page_list;

void kfree(void *pa) {
    // pa to pte
    auto *p = reinterpret_cast<free_page *>(pa);
    p->next = free_page_list;
    free_page_list = p;
}

#define PTE_V 1L << 0
#define PTE_R 1L << 1
#define PTE_W 1L << 2
#define PTE_X 1L << 3
#define PTE_U 1L << 4

#define PA2PTE(_pa)

#define PTE2PA(_pte)

#define PG_LEN 9

void free_range(void *pa_begin, void *pa_end) {
    for (char *begin = reinterpret_cast<char *>(pa_begin); begin < pa_end; begin += PAGE_SIZE) {
        kfree(begin);
    }
}

void mappage(page_table_t pt, uint64_t va, uint64_t pa, int size, int perm) {

}

void *kalloc() {
    auto page = free_page_list;
    free_page_list = free_page_list->next;
    memset(page, 7, PAGE_SIZE);
    return page;
}

page_table_t kvmmake() {
    page_table_t pt;
    kalloc();
    mappage(pt, CLINT, CLINT, PLIC - CLINT, PTE_W | PTE_R);
    mappage(pt, PLIC, PLIC, UART0 - PLIC, PTE_W | PTE_R);
    mappage(pt, UART0, UART0, VIRTIO0 - UART0, PTE_W | PTE_R);
    mappage(pt, VIRTIO0, VIRTIO0, KERNBASE - VIRTIO0, PTE_R);
    mappage(pt, KERNBASE, KERNBASE, PHYSTOP - KERNBASE, PTE_W | PTE_R);
    mappage(pt, TRAMPOLINE, TRAMPOLINE, PAGE_SIZE, PTE_W | PTE_R)
}

void kinit()
{
    free_range(end, reinterpret_cast<void *>(PHYSTOP));
}