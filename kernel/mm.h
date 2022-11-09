#pragma once
#include <stdint.h>

#define HEAP_START 0x80000000
// 128M
#define HEAP_SIZE (128 * (1 << 20))
#define PAGE_SIZE 4096
// todo: align
#define PET_CNT (HEAP_SIZE / PAGE_SIZE)
struct page_info
{
    int ref;
    int privilege;
};

using pte_t = uint64_t;
using page_table_t = uint64_t *;

struct free_page {
    free_page *next;
};

#define PTE_V 1L << 0
#define PTE_R 1L << 1
#define PTE_W 1L << 2
#define PTE_X 1L << 3
#define PTE_U 1L << 4

#define PA2PTE(_pa) (((_pa) >> 12) << 10)

#define PTE2PA(_pte) (((_pte) >> 10) << 12)

#define PX_SHIFT_LEN 9
#define PX_MASK (1 << PX_SHIFT_LEN)
#define PX_SHIFT(_va, _level) (_va >> (_level * PX_SHIFT_LEN + 12))

#define PX(_va, _level) (PX_SHIFT(_va, _level) & PX_MASK)

#define ROUND_DOWN(_a) _a
#define ROUND_UP(_a) _a


void vminit();

void kinit();

void check_memory();

// use riscv's sv39 page table scheme.
#define SATP_SV39 (8L << 60)

#define MAKE_SATP(pagetable) (SATP_SV39 | (((uint64_t)pagetable) >> 12))

static inline void
sfence_vma()
{
    // the zero, zero means flush all TLB entries.
    asm volatile("sfence.vma zero, zero");
}

// supervisor address translation and protection;
// holds the address of the page table.
static inline void
w_satp(uint64_t x)
{
    asm volatile("csrw satp, %0" : : "r" (x));
}
