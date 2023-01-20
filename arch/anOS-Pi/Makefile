##（1）Makefile中的 符号 $@, $^, $< 的意思：
#　　$@  表示目标文件
#　　$^  表示所有的依赖文件
#　　$<  表示第一个依赖文件
#　　$?  表示比目标还要新的依赖文件列表
##（2）wildcard、notdir、patsubst的意思：
#　　wildcard : 扩展通配符
#　　notdir ： 去除路径
#　　patsubst ：替换通配符
## (3) *, %
#    * means row
#    % means list

# parameter
COMPILER ?= aarch64-elf
ARCH ?= bcm2837
CFLAGS = -I. -I $(INCLUDE_DIR) -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles

# multi-folder
BUILD_DIR = build/
INCLUDE_DIR = include/
ARCH_DIR = arch/$(ARCH)/
KERNEL_DIR = kernel/
LIB_DIR = lib/
OBJ_DIR = $(BUILD_DIR)obj/

LINKER = $(ARCH_DIR)linker.ld

KERNEL = $(BUILD_DIR)spirit.img
LIST = $(BUILD_DIR)spirit.list
MAP = $(BUILD_DIR)spirit.map
ELF = $(BUILD_DIR)spirit.elf
FONT = $(OBJ_DIR)font.o

include $(ARCH_DIR)Makefile
include $(LIB_DIR)Makefile
include $(KERNEL_DIR)Makefile

OBJECT += $(FONT)

all: $(OBJECT) $(LINKER)
	#mkdir -p $(OBJ_DIR)
	$(COMPILER)-ld -r -b binary -o $(FONT) $(BUILD_DIR)font.psf
	$(COMPILER)-ld -nostdlib -nostartfiles $(OBJECT) -Map $(MAP) -o $(ELF) -T $(LINKER)
	$(COMPILER)-objcopy $(ELF) -O binary $(KERNEL)
	$(COMPILER)-objdump -D -x $(ELF) > $(LIST)
	-python3 tool/versioncounter.py document/VERSION

$(FONT):
	mkdir -p $(OBJ_DIR)
	$(COMPILER)-ld -r -b binary -o $(FONT) $(BUILD_DIR)font.psf

#all: $(KERNEL) $(LIST)
	#-python3 version.py VERSION

#$(LIST): $(ELF)
	#$(COMPILER)-objdump -D -x $(ELF) > $(LIST)

#$(KERNEL): $(ELF)
	#$(COMPILER)-objcopy $(ELF) -O binary $(KERNEL)

#$(ELF): $(OBJECT) $(LINKER)
	#$(COMPILER)-ld -nostdlib -nostartfiles $(OBJECT) -Map $(MAP) -o $(ELF) -T $(LINKER)

#$(FONT):
	#mkdir -p $(OBJ_DIR)
	#$(COMPILER)-ld -r -b binary -o $(FONT) build/obj/font.psf
	
#$(OS).bin: $(OBJS) ${LDS}
	#mkdir -p ${BUILD_DIR}
	#$(LD) $(OBJS) $(LDFLAGS) -T ${LDS} -o ${BUILD_DIR}/$(OS).elf
	#$(OBJCOPY) -O binary ${BUILD_DIR}/$(OS).elf ${BUILD_DIR}/$(OS).bin
	#$(OBJDUMP) -D ${BUILD_DIR}/$(OS).elf > ${BUILD_DIR}/$(OS).asm

clean: 
	-rm -f $(OBJECT)
	-rm -f $(TARGET)
	-rm -f $(LIST)
	-rm -f $(MAP)
	-rm -f $(ELF)

mount:
	cp build/spirit.img /mnt/c/com/kernel8.img

qemu:
	qemu-system-aarch64 -nographic -m 512 -M raspi3 -kernel build/spirit.img
