#include "os.h"

   .text 
_start:
   //MBR
	mov $0, %ax				
	mov %ax, %ds			
	mov %ax, %es			
	mov %ax, %ss			
	mov $_start, %esp		
   jmp .

   mov $0x7E00, %bx  // 读取到的内存地址
   mov $0x20, %cx    // ch:磁道号，cl起始扇区号
   mov $0x240, %ax   // ah: 0x42读磁盘命令, al=0x40 64个扇区，多读一些, 32KB
   mov $0x80 , %dx   // dh: 磁头号，dl驱动器号0x80(磁盘1)
   int $0x0013
   //jc read_self_all  // 读取失败就重复读写

   //jmp .

   .org 0x1fe        //510
   .byte 0x55, 0xaa  //MBR魔数

   
   .fill 64*1024, 1, 0x35