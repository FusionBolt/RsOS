CC = riscv64-unknown-elf-gcc
CFLAGS = -nostdlib -fno-builtin -mcmodel=medany -march=rv64ima -mabi=lp64

QEMU = qemu-system-riscv64
QFLAGS = -nographic -smp 4 -machine virt -bios none

OBJDUMP = riscv64-unknown-elf-objdump

all: os.elf

os.elf: boot.s os.cpp string.cpp sys.s libc.cpp proc.cpp trap.cpp mm.cpp ../user/shell.cpp plic.cpp start.cpp
	$(CC) $(CFLAGS) -T linker.ld -o os.elf $^
	riscv64-unknown-elf-objdump -d os.elf > os.asm

qemu: $(TARGET)
	@qemu-system-riscv64 -M ? | grep virt >/dev/null || exit
	@echo "Press Ctrl-A and then X to exit QEMU"
	$(QEMU) $(QFLAGS) -kernel os.elf

clean:
	rm -f *.elf
