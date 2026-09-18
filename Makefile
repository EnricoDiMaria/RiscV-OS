CC = riscv64-elf-gcc
OBJCOPY = riscv64-elf-objcopy
CFLAGS += -march=rv64gc -mabi=lp64d -mcmodel=medany -ffreestanding -nostdlib -fno-builtin -mstrict-align -g -Wall -Wextra
LDFLAGS = -T linker.ld -nostdlib -nostartfiles

KERNEL_OBJS = boot.o trap.o kernel.o kfunctions.o plic.o UART.o common.o switch.o process.o virtual.o

all: kernel.elf

#Kernel
kernel.elf: $(KERNEL_OBJS) shell.bin.o
	$(CC) $(LDFLAGS) $(KERNEL_OBJS) shell.bin.o -o kernel.elf

#User
shell.elf: shell.o user.o user_s.o common.o
	$(CC) $(CFLAGS) -Wl,-Tuser.ld -Wl,-Map=shell.map -o shell.elf shell.o user.o user_s.o common.o

shell.bin: shell.elf
	$(OBJCOPY) --set-section-flags .bss=alloc,contents -O binary shell.elf shell.bin

shell.bin.o: shell.bin
	$(OBJCOPY) -I binary -O elf64-littleriscv shell.bin shell.bin.o

# C compiling
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Assembly compiling
%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

run: kernel.elf
	qemu-system-riscv64 -machine virt -bios none -kernel kernel.elf -nographic 

debug: kernel.elf
	qemu-system-riscv64 -machine virt -nographic -bios none -kernel kernel.elf -s -S

clean:
	rm -f *.o *.elf *.bin *.map