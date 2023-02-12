BUILD_DIR=build/
BOOT_DIR=boot/
KERNEL_DIR=kernel/

.PHONY:

Bochs: $(BUILD_DIR) Compile
	bximage -func=create -fd=1.44M -imgmode=flat -sectsize=512 -q $(BUILD_DIR)bootloader.img
	#bximage -func=create -hd=10M -imgmode=flat -sectsize=512 -q $(BUILD_DIR)bootloader.img
	@echo "============================"

	dd if=$(BUILD_DIR)boot.bin of=$(BUILD_DIR)bootloader.img bs=512 count=1 conv=notrunc
	@echo "============================"

	mkdir bootloader/
	sudo mount $(BUILD_DIR)bootloader.img bootloader/ -t vfat -o loop
	rm -rf bootloader/*
	sudo cp $(BUILD_DIR)loader.bin bootloader/
	sudo cp $(BUILD_DIR)kernel.bin bootloader/
	sync
	sudo umount bootloader/
	rm -r bootloader/
	@echo "============================"

	bochs -qf .bochsrc

Compile: $(BUILD_DIR)
	make -C $(BOOT_DIR)
	make -C $(KERNEL_DIR)

$(BUILD_DIR):
	if [ ! -d $@ ]; then mkdir $@; fi

clean:
	rm -r $(BUILD_DIR)