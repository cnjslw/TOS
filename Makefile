BOOT_DIR_SRC =  source/boot/
BOOT_DIR_INC =  source/boot/include
BUILD_DIR_BOOT = build/boot

IMG_DIR = image

KER_DIR = source/kernel
BUILD_DIR_KER = build/kernel

all: $(BOOT_DIR_SRC)/start.S $(BOOT_DIR_SRC)/loader.S 
	nasm -I $(BOOT_DIR_INC) -o $(BUILD_DIR_BOOT)/start.bin $(BOOT_DIR_SRC)/start.S
	nasm -I $(BOOT_DIR_INC) -o $(BUILD_DIR_BOOT)/load.bin $(BOOT_DIR_SRC)/loader.S
	gcc -c -o $(BUILD_DIR_KER)/main.o $(KER_DIR)/main.c 
	ld $(BUILD_DIR_KER)/main.o -Ttext 0xc0001500 -e main -o $(BUILD_DIR_KER)/kernel.bin

clean:
	rm -f $(BUILD_DIR_BOOT)/* $(BUILD_DIR_KER)/*

run:
	rm -rf $(IMG_DIR)/disk.img
	rm -rf $(IMG_DIR)/disk.img.lock
	bximage -hd=10M -mode="create" -q $(IMG_DIR)/disk.img

	dd if=./$(BUILD_DIR_BOOT)/start.bin of=./$(IMG_DIR)/disk.img bs=512 count=1 conv=notrunc
	dd if=./$(BUILD_DIR_BOOT)/load.bin of=./$(IMG_DIR)/disk.img bs=512 count=4 seek=2 conv=notrunc
	dd if=./$(BUILD_DIR_KER)/kernel.bin of=./$(IMG_DIR)/disk.img bs=512 count=200 seek=9 conv=notrunc
	
	bochs/bin/bochs -f bochs/bin/bochsrc.disk 