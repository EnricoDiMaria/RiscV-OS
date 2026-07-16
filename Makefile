CC = riscv64-elf-gcc
CFLAGS += -march=rv64gc -mabi=lp64d -mcmodel=medany -ffreestanding -nostdlib -fno-builtin -mstrict-align -g
LDFLAGS = -T linker.ld -nostdlib -nostartfiles

OBJS = boot.o trap.o kernel.o plic.o UART.o common.o

all: kernel.elf

kernel.elf: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o kernel.elf

#C compiling
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

#Assembly compiling
%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

run: kernel.elf
	qemu-system-riscv64 -machine virt -bios none -kernel kernel.elf -nographic 

debug: kernel.elf
	qemu-system-riscv64 -machine virt -nographic -bios none -kernel kernel.elf -s -S

clean:
	rm -f *.o kernel.elf
