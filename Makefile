# build directory \
naming rule BUILD_<modulename>
BUILD_BOOT = build/boot
BUILD_DEV  = build/device
BUILD_KER  = build/kernel
BUILD_LIB  = build/lib

# source directory \
naming rule SRC_<modulename>
SRC_BOOT = source/boot
SRC_KER  = source/kernel
SRC_LIB  = source/lib
SRC_DEV  = device

# library directory \
naming rule LIB_<modulename>
LIB_ROOT = source/lib
LIB_KER  = source/lib/kernel

# image directory
IMG_DIR = image


# Compiler Options
ENTRY_POINT = 0xc0001500
AS = nasm
CC = gcc
LD = ld
LIB = -I $(LIB_KER) -I $(LIB_ROOT) -I $(SRC_DEV) -I $(SRC_BOOT) -I $(SRC_KER)
ASFLAGS = -f elf
ASBINLIB = -I source/boot/include/
CFLAGS  = -Wall $(LIB) -c -m32 -fno-builtin -fno-stack-protector -W -Wstrict-prototypes -Wmissing-prototypes 
OBJS = $(BUILD_KER)/main.o $(BUILD_KER)/init.o $(BUILD_KER)/interrupt.o \
      $(BUILD_DEV)/timer.o $(BUILD_KER)/kernel.o $(BUILD_LIB)/print.o \
      $(BUILD_KER)/debug.o 
LDFLAGS = -m elf_i386 -Ttext $(ENTRY_POINT) -e main -Map $(BUILD_KER)/kernel.map

##############     MBR代码编译     ############### 
$(BUILD_BOOT)/mbr.bin: $(SRC_BOOT)/mbr.S 
	$(AS) $(ASBINLIB) $< -o $@

##############     bootloader代码编译     ###############
$(BUILD_BOOT)/loader.bin: $(SRC_BOOT)/loader.S 
	$(AS) $(ASBINLIB) $< -o $@


#################### c代码编译 ##########################
$(BUILD_KER)/main.o: $(SRC_KER)/main.c $(SRC_KER)/init.h \
				   $(LIB_ROOT)/stdint.h $(LIB_KER)/print.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_KER)/init.o: $(SRC_KER)/init.c $(SRC_KER)/init.h $(LIB_KER)/print.h \
        $(LIB_ROOT)/stdint.h $(SRC_KER)/interrupt.h $(SRC_DEV)/timer.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_KER)/interrupt.o: $(SRC_KER)/interrupt.c $(SRC_KER)/interrupt.h \
        $(LIB_ROOT)/stdint.h $(SRC_KER)/global.h $(LIB_KER)/io.h $(LIB_KER)/print.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DEV)/timer.o: $(SRC_DEV)/timer.c $(SRC_DEV)/timer.h $(LIB_ROOT)/stdint.h\
         $(LIB_KER)/io.h $(LIB_KER)/print.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_KER)/debug.o: $(SRC_KER)/debug.c $(SRC_KER)/debug.h \
        $(LIB_KER)/print.h $(LIB_ROOT)/stdint.h $(SRC_KER)/interrupt.h
	$(CC) $(CFLAGS) $< -o $@


##############    汇编代码编译    ###############
$(BUILD_KER)/kernel.o: $(SRC_KER)/kernel.S
	$(AS) $(ASFLAGS) $< -o $@
$(BUILD_LIB)/print.o: $(LIB_KER)/print.S
	$(AS) $(ASFLAGS) $< -o $@

##############    链接所有目标文件    #############
$(BUILD_KER)/kernel.bin: $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@


.PHONY:build run clean

build:$(BUILD_KER)/kernel.bin $(BUILD_BOOT)/loader.bin $(BUILD_BOOT)/mbr.bin

run: 
	rm -rf $(IMG_DIR)/disk.img
	rm -rf $(IMG_DIR)/disk.img.lock
	bximage -hd=10M -mode="create" -q $(IMG_DIR)/disk.img
	dd if=./$(BUILD_BOOT)/mbr.bin of=./$(IMG_DIR)/disk.img bs=512 count=1 conv=notrunc
	dd if=./$(BUILD_BOOT)/loader.bin of=./$(IMG_DIR)/disk.img bs=512 count=4 seek=2 conv=notrunc
	dd if=./$(BUILD_KER)/kernel.bin of=./$(IMG_DIR)/disk.img bs=512 count=200 seek=9 conv=notrunc
	bochs/bin/bochs -f bochs/bin/bochsrc.disk 

clean:
	-rm -r $(BUILD_BOOT)/* $(BUILD_DEV)/* $(BUILD_KER)/* $(BUILD_LIB)/*

all:build run

