#include "os.h"
#include "riscv.h"
#include "libc.h"
#include "proc.h"
#include "memlayout.h"

extern "C" {
void trap_vector();
void kernelvec();

void kerneltrap()
{
//    lib_printf("kernel trap\n");
    uint64_t sepc = r_sepc();
    uint64_t sstatus = r_sstatus();
    uint64_t scause = r_scause();

    if((sstatus & SSTATUS_SPP) == 0) {
        panic("kerneltrap: not from supervisor mode");
    }
    if(intr_get() != 0) {
        panic("kerneltrap: interrupts enabled");
    }

    w_sepc(sepc);
    w_sstatus(sstatus);
}

void trap_init()
{
    w_stvec(reinterpret_cast<reg_t>(kernelvec));
}

void preemptive(reg_t);

int plic_claim() {
    int hart = r_tp();
    int irq = *(uint32_t *) PLIC_MCLAIM(hart);
    return irq;
}

void plic_complete(int irq) {
    int hart = r_tp();
    *(uint32_t *) PLIC_MCOMPLETE(hart) = irq;
}

void devintr() {
    int irq = plic_claim();
    if (irq == UART0_IRQ) {
        lib_isr();
    }
    //    else if (irq == VIRTIO_IRQ)
    //    {
    //        lib_puts("Virtio IRQ\n");
    //        virtio_disk_isr();
    //    }
    else if (irq) {
        lib_printf("unexpected interrupt irq = %d\n", irq);
    }

    if (irq) {
        plic_complete(irq);
    }
}


reg_t trap_handler(reg_t epc, reg_t cause) {
    auto sstatus = r_sstatus();

    if((r_sstatus() & SSTATUS_SPP) != 0)
        panic("usertrap: not from user mode");

    // set S Previous Privilege mode to User.
//    unsigned long x = r_sstatus();
//    x &= ~SSTATUS_SPP; // clear SPP to 0 for user mode
//    x |= SSTATUS_SPIE; // enable interrupts in user mode
//    w_sstatus(x);

    if ((sstatus & SSTATUS_SPP) == 0)
        panic("kerneltrap: not from supervisor mode");
    if (intr_get() != 0)
        panic("kerneltrap: interrupts enabled");

    reg_t ret_pc = epc;
    //    reg_t cause_code = cause & 0xfff;
    reg_t cause_code = r_scause();
    //    lib_printf("cause:%x\n", cause);
    if (cause & 0x80000000) {
        lib_printf("cause:%x\n", cause);
    }
    if (cause & 0x80000000) {
        /* Asynchronous trap - interrupt */
        switch (cause_code) {
            case 3:
                //                lib_puts("software interruption!\n");
                break;
            case 7:
                //                lib_puts("timer interruption!\n");
                // disable machine-mode timer interrupts.
                w_mie(~((~r_mie()) | (1 << 7)));
//                timer_handler();
                ret_pc = epc;
                //                ret_pc = (reg_t) &preemptive;
                // enable machine-mode timer interrupts.
                w_mie(r_mie() | MIE_MTIE);
                break;
            case 11:
                devintr();
                break;
            default:
                lib_printf("unknown async exception: %d\n", cause_code);
                break;
        }
    } else {
        switch (cause_code) {
            case 2:
                lib_puts("Illegal instruction!\n");
                break;
            case 5:
                lib_puts("Fault load!\n");
                break;
            case 7:
                lib_puts("Fault store!\n");
                break;
            case 8:
                lib_puts("Environment call from U-mode!\n");
                //                do_syscall(ctx);
                ret_pc += 4;
                break;
            case 11:
                lib_puts("Environment call from M-mode!\n");
                //                do_syscall(ctx);
                ret_pc += 4;
                break;
            default:
                /* Synchronous trap - exception */
                lib_printf("Sync exceptions! cause code: %d\n", cause_code);
                while (1);
                break;
        }
    }

    w_mie(r_mie() | MIE_MTIE);
    return ret_pc;
}

//void preemptive(reg_t epc) {
//    lib_printf("epc:%x\n", epc);
//    // 除非提前保存pc
//    //在这里开始回去调度，那么一会回来的也是这里，无法再回到user proc了
//    delay(10000);
//    lib_printf("pid: %d ", current_proc()->pid);
//    //    current_proc()->time = timer_count;
//    //    timer_count = 0;
//    current_proc()->state = proc_sleeping;
//    lib_puts("preemptive!\n");
//
//    switch_to_sys_proc();
//    lib_puts("end preemtive\n");
//
//    //    lib_printf("2 status:%d\n", sys_proc()[2].state);
//    //    run_proc(sys_proc());
//}
}