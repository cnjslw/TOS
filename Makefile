all: source/start.S source/loader.S
	nasm -I source/include/ -o build/start.bin source/start.S
	nasm -I source/include/ -o build/load.bin source/loader.S
	
clean:
	rm -f build/*.elf build/*.o build/*.txt build/*.bin

run:
	rm -rf image/disk.img.lock
	dd if=./build/start.bin of=./image/disk.img bs=512 count=1 conv=notrunc
	dd if=./build/load.bin of=./image/disk.img bs=512 count=1 seek=2 conv=notrunc
	bochs/bin/bochs -f bochs/bin/bochsrc.disk 