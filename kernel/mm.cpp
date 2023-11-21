#include "mm.h"
#include "os.h"
#include "memlayout.h"
#include "string.h"
#include "libc.h"
extern char _memory_start[];
extern char _heap_start[];

page_table_t kernel_page_table;
free_page *free_page_list;

void kfree(void *pa) {
    // pa to pte
    auto *p = reinterpret_cast<free_page *>(pa);
    p->next = free_page_list;
    free_page_list = p;
}

void *kalloc() {
    if(!free_page_list) {
        return nullptr;
//        panic("free_page_list is nullptr");
    }
//    lib_printf("kalloc\n");
    auto page = free_page_list;
    free_page_list = free_page_list->next;
    return page;
}

void free_range(void *pa_begin, void *pa_end) {
    lib_printf("begin: %x\n", pa_begin);
    lib_printf("end: %x\n", pa_end);
    // todo： 不round就会出错
    lib_printf("round begin: %x\n", PGROUNDUP((uint64_t)pa_begin));
    lib_printf("round end: %x\n", PGROUNDDOWN((uint64_t)pa_end));
    for (char *begin = reinterpret_cast<char *>(PGROUNDUP((uint64_t)pa_begin)); begin < (char *)PGROUNDDOWN((uint64_t)pa_end); begin += PAGE_SIZE) {
//        lib_printf("now: %x\n", begin);
        kfree(begin);
//        lib_printf("free ok\n");
    }
//    lib_printf("free range end\n");
}

void printf_va(uint64_t va) {
    lib_printf("va %d %d %d %d\n", PX(va, 2), PX(va, 1), PX(va, 0), va & (1 << 12));
}

void printf_pte_perm(pte_t *pte) {
    auto value = *pte;
    if(value & PTE_V) {
        lib_printf("v");
    }
    if(value & PTE_R) {
        lib_printf("r");
    }
    if(value & PTE_W) {
        lib_printf("w");
    }
    if(value & PTE_X) {
        lib_printf("x");
    }
    if(value & PTE_U) {
        lib_printf("u");
    }
    lib_printf("\n");
}

// va -> pte
pte_t *walk(page_table_t pt, uint64_t va, bool alloc) {
//    lib_printf("pt:%x va:%x\n", pt, va);
//    printf_va(va);
    pte_t *pte;
    for(int level = 2; level >= 0; --level) {
//        lib_printf("level:%d\n", level);
        // find pte, get addr of pte
//        lib_printf("------\n");
        pte = reinterpret_cast<pte_t *>(&(pt[PX(va, level)]));
//        lib_printf("pte_addr:%x pte_value:%x\n", pte, *pte);
//        printf_pte_perm(pte);
        // valid pte
        if(*pte & PTE_V) {
            pt = reinterpret_cast<page_table_t >(*pte);
//            lib_printf("Valid pte\n");
        } // alloc new page
        else if((pt = reinterpret_cast<page_table_t>(kalloc()))) {
            // set pte
            // result of kalloc is a physical addr which begin of page
            *pte = reinterpret_cast<uint64_t>(pt) | PTE_V;
//            lib_printf("pte_addr:%x pte_value:%x\n", pte, *pte);
//            lib_printf("set pte, pt:%x pte:%x\n", pt, *pte);
//            lib_printf("kalloc pt\n");
        }
        else {
//            lib_printf("pt:%x va:%x level:%d\n", pt, va, level);
            panic("walk failed");
            return nullptr;
        }
//        lib_printf("pte_addr:%x pte_value:%x\n", pte, *pte);
//        printf_pte_perm(pte);
    }
    return pte;
}

void mappage(page_table_t pt, uint64_t va, uint64_t pa, int size, int perm) {
//    lib_printf("mappage va:%x pa:%x size:%d perm:%x\n", va, pa, size, perm);
    pa = ROUND_DOWN(pa);
    va = ROUND_DOWN(va);
    auto end = ROUND_UP(va + size);
    for (; va < end; va += PAGE_SIZE) {
        auto *pte = walk(pt, va, 1);
//        lib_printf("pte addr:%x pte value:%x\n", pte, *pte);
//        printf_pte_perm(pte);
        if(!pte) {
            panic("empty pte");
        }
        if(!(*pte & PTE_V)) {
            panic("pte is not valid");
        }
        *pte = PA2PTE(pa) | perm | PTE_V;
    }
}

page_table_t kvmmake() {
//    lib_printf("kvmmake\n");
    page_table_t pt = reinterpret_cast<page_table_t>(kalloc());
//    lib_printf("pt:%x\n", pt);
    auto mem_start_addr = reinterpret_cast<uint64_t>(_heap_start);
//    mappage(pt, CLINT, CLINT, PAGE_SIZE, PTE_W | PTE_R);
    mappage(pt, UART0, UART0, PAGE_SIZE, PTE_W | PTE_R);
    mappage(pt, VIRTIO0, VIRTIO0, PAGE_SIZE, PTE_R);
    mappage(pt, PLIC, PLIC, 0x400000, PTE_W | PTE_R);
    mappage(pt, KERNBASE, KERNBASE, mem_start_addr - KERNBASE, PTE_X | PTE_R);
    mappage(pt, mem_start_addr, mem_start_addr, PHYSTOP - mem_start_addr, PTE_W | PTE_R | PTE_X);
    return pt;
}

void kinit()
{
    free_range(_heap_start, reinterpret_cast<void *>(PHYSTOP));
    lib_printf("heap start:%x, end:%x, size:%d\n", _heap_start, PHYSTOP, PHYSTOP - reinterpret_cast<uint64_t>(_heap_start));
}

void vminithart() {
    // wait for any previous writes to the page table memory to finish.
    sfence_vma();

    w_satp(MAKE_SATP(kernel_page_table));

    // flush stale entries from the TLB.
    sfence_vma();
}

void vminit()
{
    kernel_page_table = kvmmake();
}

void check_memory()
{

}

// create an empty user page table.
// returns 0 if out of memory.
page_table_t uvmcreate()
{
    page_table_t table;
    table = (page_table_t)kalloc();
    memset(table, 0, PAGE_SIZE);
    return table;
}

void uvmfirst(page_table_t pagetable, unsigned char *src, uint64_t size)
{
    if(size >= PAGE_SIZE) {
        panic("uvm first: need more than a page");
    }

    // todo: kalloc分配了一个页的大小，是物理地址吗？？
    auto mem = kalloc();
    memset(mem, 0, size);
    mappage(pagetable, 0, reinterpret_cast<uint64_t>(mem), PAGE_SIZE, PTE_R | PTE_W | PTE_X | PTE_U);
    // memcpy假设不能有overlap
    // memmove没有这个假设
    // todo: 这里会有overlap？
    memmove(mem, src, size);
}

void uvmalloc()
{

}

void uvmfree()
{

}