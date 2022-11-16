#include "riscv.h"
#include "os.h"
#include <stdint.h>
#include "memlayout.h"

extern "C" {
void plic_init() {
    // set desired IRQ priorities non-zero (otherwise disabled).
    *(uint32_t *) (PLIC + UART0_IRQ * 4) = 1;
    *(uint32_t *) (PLIC + VIRTIO0_IRQ * 4) = 1;


    int hart = cpuid();

    // set enable bits for this hart's S-mode
    // for the uart and virtio disk.
    *(uint32_t *) PLIC_SENABLE(hart) = (1 << UART0_IRQ) | (1 << VIRTIO0_IRQ);

    // set this hart's S-mode priority threshold to 0.
    *(uint32_t *) PLIC_SPRIORITY(hart) = 0;
}
}