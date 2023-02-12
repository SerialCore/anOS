BUILD_DIR=../build/

KERNEL=$(BUILD_DIR)kernel.bin
ELF=kernel.elf
LIST=kernel.list
MAP=kernel.map
LINKER=Kernel.lds

all: ELF
	objcopy -I elf64-x86-64 -S -R ".eh_frame" -R ".comment" $< -O binary $(KERNEL)
	objdump -D -x $< > $(LIST)

ELF: head.o main.o
	ld -b elf64-x86-64 $^ -Map $(MAP) -o $@ -T $(LINKER)

head.o: head.S
	gcc -E $< > head.s
	as --64 -o $@ head.s

main.o: main.c
	gcc -mcmodel=large -fno-builtin -m64 -c $< -o $@