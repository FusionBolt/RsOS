#pragma once

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
void init_page_info();
void kinit();