# Bootloader 源码与生成文件的目录

ROOT_DIR =

BOOT_DIR_SRC =  $(ROOT_DIR)source/boot#loader、start 源文件
BOOT_DIR_INC =  $(ROOT_DIR)source/boot/include#Boot头文件
BUILD_DIR_BOOT = $(ROOT_DIR)build/boot#可执行文件的生成目录

# 映像文件目录
IMG_DIR = $(ROOT_DIR)image

# Kernel 源码与生成文件的目录
KER_DIR = $(ROOT_DIR)source/kernel# main 源文件
BUILD_DIR_KER = $(ROOT_DIR)build/kernel# main 生成目录

# 库文件
LIB_KER = $(ROOT_DIR)source/lib/kernel# 库文件源代码目录
LIB_ROOT = $(ROOT_DIR)source/lib# 未归档库文件
LIB_KER_BUILD = $(ROOT_DIR)build/lib/kernel# kernel库文件生成

all: source/boot/start.S source/boot/loader.S 
	nasm -I $(BOOT_DIR_INC) -o $(BUILD_DIR_BOOT)/start.bin $(BOOT_DIR_SRC)/start.S
	nasm -I $(BOOT_DIR_INC) -o $(BUILD_DIR_BOOT)/load.bin $(BOOT_DIR_SRC)/loader.S
	nasm -f elf -o $(LIB_KER_BUILD)/print.o $(LIB_KER)/print.S
	gcc -m32 -I $(LIB_KER)/ -I $(LIB_ROOT)/ -c -o $(BUILD_DIR_KER)/main.o $(KER_DIR)/main.c 
#   ld -melf_i386  -Ttext 0xc0001500 -e main -o ./out/kernel/kernel.bin out/kernel/main.o out/kernel/print.o
#	这是一个Linux下的ld链接器命令，用于将两个目标文件（main.o和print.o）链接成一个可执行文件kernel.bin。
#   以下是参数的详细解释：
#   -melf_i386：指定输出的目标文件格式为ELF i386。
#   -Ttext 0xc0001500：指定链接后可执行文件的起始地址是0xc0001500。
#   -e main：告诉链接器程序的入口点是main函数。
#   -o ./out/kernel/kernel.bin：指定输出文件名和路径。
#   out/kernel/main.o out/kernel/print.o：要链接的目标文件列表。
	ld -melf_i386   -Ttext 0xc0001500 -e main -o $(BUILD_DIR_KER)/kernel.bin $(BUILD_DIR_KER)/main.o $(LIB_KER_BUILD)/print.o

clean:
	rm -r $(BUILD_DIR_BOOT)/* $(BUILD_DIR_KER)/* $(LIB_KER_BUILD)

run:
	rm -rf $(IMG_DIR)/disk.img
	rm -rf $(IMG_DIR)/disk.img.lock
	bximage -hd=10M -mode="create" -q $(IMG_DIR)/disk.img

	dd if=./$(BUILD_DIR_BOOT)/start.bin of=./$(IMG_DIR)/disk.img bs=512 count=1 conv=notrunc
	dd if=./$(BUILD_DIR_BOOT)/load.bin of=./$(IMG_DIR)/disk.img bs=512 count=4 seek=2 conv=notrunc
	dd if=./$(BUILD_DIR_KER)/kernel.bin of=./$(IMG_DIR)/disk.img bs=512 count=200 seek=9 conv=notrunc
	
	bochs/bin/bochs -f bochs/bin/bochsrc.disk 