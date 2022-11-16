#pragma once
#include <stdint.h>
using reg_t = uint64_t;

struct proc_ctxt {
    reg_t ra;
    reg_t sp;
    reg_t s0;
    reg_t s1;
    reg_t s2;
    reg_t s3;
    reg_t s4;
    reg_t s5;
    reg_t s6;
    reg_t s7;
    reg_t s8;
    reg_t s9;
    reg_t s10;
    reg_t s11;
};


// ================== Timer Interrput ====================

#define NCPU 8             // maximum number of CPUs
#define CLINT 0x2000000
#define CLINT_MTIMECMP(hartid) (CLINT + 0x4000 + 4*(hartid))
#define CLINT_MTIME (CLINT + 0xBFF8) // cycles since boot.

// which hart (core) is this?
static inline reg_t r_mhartid()
{
    reg_t x;
    asm volatile("csrr %0, mhartid" : "=r" (x) );
    return x;
}

// Machine Status Register, mstatus
#define MSTATUS_MPP_MASK (3 << 11) // previous mode.
#define MSTATUS_MPP_M (3 << 11)
#define MSTATUS_MPP_S (1 << 11)
#define MSTATUS_MPP_U (0 << 11)
#define MSTATUS_MIE (1 << 3)    // machine-mode interrupt enable.

static inline reg_t r_mstatus()
{
    reg_t x;
    asm volatile("csrr %0, mstatus" : "=r" (x) );
    return x;
}

static inline void w_mstatus(reg_t x)
{
    asm volatile("csrw mstatus, %0" : : "r" (x));
}

// machine exception program counter, holds the
// instruction address to which a return from
// exception will go.
static inline void w_mepc(reg_t x)
{
    asm volatile("csrw mepc, %0" : : "r" (x));
}

static inline reg_t r_mepc()
{
    reg_t x;
    asm volatile("csrr %0, mepc" : "=r" (x));
    return x;
}

// Machine Scratch register, for early trap handler
static inline void w_mscratch(reg_t x)
{
    asm volatile("csrw mscratch, %0" : : "r" (x));
}

// Machine-mode interrupt vector
static inline void w_mtvec(reg_t x)
{
    asm volatile("csrw mtvec, %0" : : "r" (x));
}

static inline void w_stvec(reg_t x)
{
    asm volatile("csrw stvec, %0" : : "r" (x));
}

// Machine-mode Interrupt Enable
#define MIE_MEIE (1 << 11) // external
#define MIE_MTIE (1 << 7)  // timer
#define MIE_MSIE (1 << 3)  // software

static inline reg_t r_mie()
{
    reg_t x;
    asm volatile("csrr %0, mie" : "=r" (x) );
    return x;
}

static inline void w_mie(reg_t x)
{
    asm volatile("csrw mie, %0" : : "r" (x));
}

// read and write tp, the thread pointer, which xv6 uses to hold
// this core's hartid (core number), the index into cpus[].
static inline uint64_t r_tp()
{
    uint64_t x;
    asm volatile("mv %0, tp" : "=r" (x) );
    return x;
}

static inline void w_tp(uint64_t x)
{
    asm volatile("mv tp, %0" : : "r" (x));
}


// Supervisor Status Register, sstatus

#define SSTATUS_SPP (1L << 8)  // Previous mode, 1=Supervisor, 0=User
#define SSTATUS_SPIE (1L << 5) // Supervisor Previous Interrupt Enable
#define SSTATUS_UPIE (1L << 4) // User Previous Interrupt Enable
#define SSTATUS_SIE (1L << 1)  // Supervisor Interrupt Enable
#define SSTATUS_UIE (1L << 0)  // User Interrupt Enable

static inline uint64_t r_sstatus()
{
    uint64_t x;
    asm volatile("csrr %0, sstatus" : "=r" (x) );
    return x;
}

static inline void w_sstatus(uint64_t x)
{
    asm volatile("csrw sstatus, %0" : : "r" (x));
}


// are device interrupts enabled?
static inline int intr_get()
{
    uint64_t x = r_sstatus();
    return (x & SSTATUS_SIE) != 0;
}

// Supervisor Trap Cause
static inline uint64_t r_scause()
{
    uint64_t x;
    asm volatile("csrr %0, scause" : "=r" (x) );
    return x;
}

// Must be called with interrupts disabled,
// to prevent race with process being moved
// to a different CPU.
inline int cpuid()
{
    int id = r_tp();
    return id;
}


static inline void
w_medeleg(uint64_t x)
{
    asm volatile("csrw medeleg, %0" : : "r" (x));
}

static inline void w_mideleg(uint64_t x)
{
    asm volatile("csrw mideleg, %0" : : "r" (x));
}

// Supervisor Interrupt Enable
#define SIE_SEIE (1L << 9) // external
#define SIE_STIE (1L << 5) // timer
#define SIE_SSIE (1L << 1) // software
static inline uint64_t r_sie()
{
    uint64_t x;
    asm volatile("csrr %0, sie" : "=r" (x) );
    return x;
}

static inline void
w_sie(uint64_t x)
{
    asm volatile("csrw sie, %0" : : "r" (x));
}


// Physical Memory Protection
static inline void
w_pmpcfg0(uint64_t x)
{
    asm volatile("csrw pmpcfg0, %0" : : "r" (x));
}

static inline void
w_pmpaddr0(uint64_t x)
{
    asm volatile("csrw pmpaddr0, %0" : : "r" (x));
}

// ref: https://www.activexperts.com/serial-port-component/tutorials/uart/
#define UART 0x10000000L
#define UART_THR (volatile uint8_t *)(UART + 0x00) // THR:transmitter holding register
#define UART_RHR (volatile uint8_t *)(UART + 0x00) // RHR:Receive holding register
#define UART_DLL (volatile uint8_t *)(UART + 0x00) // LSB of Divisor Latch (write mode)
#define UART_DLM (volatile uint8_t *)(UART + 0x01) // MSB of Divisor Latch (write mode)
#define UART_IER (volatile uint8_t *)(UART + 0x01) // Interrupt Enable Register
#define UART_LCR (volatile uint8_t *)(UART + 0x03) // Line Control Register
#define UART_LSR (volatile uint8_t *)(UART + 0x05) // LSR:line status register
#define UART_LSR_EMPTY_MASK 0x40                   // LSR Bit 6: Transmitter empty; both the THR and LSR are empty

