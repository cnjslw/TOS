CFLAGS = -g -c -O0 -m32 -fno-pie -fno-stack-protector -nostdlib -nostdinc

all: source/os.c source/os.h source/start.S
	gcc $(CFLAGS) source/start.S build/
	gcc $(CFLAGS) source/os.c build/
	ld -m elf_i386 -Ttext=0x7c00 start.o os.o -o build/os.elf
	objcopy -O binary build/os.elf build/os.bin
	objdump -x -d -S  build/os.elf > build/os_dis.txt	
	readelf -a  build/os.elf > build/os_elf.txt	
	dd if=build/os.bin of=./image/disk.img conv=notrunc

clean:
	rm -f build/*.elf build/*.o build/*.txt build/*.bin