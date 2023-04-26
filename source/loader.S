%include "boot.inc"
SECTION loader vstart=LOADER_BASE_ADDR
LOADER_STACK_TOP equ LOADER_BASE_ADDR
jmp loader_start

;构建gdt
GDT_BASE: dd 0x00000000
          dd 0x00000000

CODE_DESC: dd 0x0000FFFF
           dd DESC_CODE_HIGH4

DATA_STACK_DESC: dd 0x0000FFFF
                 dd DESC_VIDEO_HIGH4
            
VIDEO_DESC: dd 0x8000000007
            dd DESC_VIDEO_HIGH4

GDT_SIZE   equ $-GDT_BASE
GDT_LIMIT  equ GDT_SIZE -1
times 60 dq 0
SELECTOR_CODE equ (0x0001<<3) + TI_GDT + RPL0
SELECTOR_DATA equ (0x0002<<3) + TI_GDT + RPL0
SELECTOR_VIDEO equ (0x0003<<3) + TI_GDT + RPL0

gdt_ptr dw GDT_LIMIT ;双字，16位
        dd GDT_BASE  ;双字，32位
loadermsg db '2 loader in real'

loader_start:
    mov byte [gs:0x20],'2'
    mov byte [gs:0x21],0xA4     ; A表示绿色背景闪烁，4表示前景色为红色

    mov byte [gs:0x22],' '
    mov byte [gs:0x23],0xA4

    mov byte [gs:0x24],'L'
    mov byte [gs:0x25],0xA4   

    mov byte [gs:0x26],'O'
    mov byte [gs:0x27],0xA4

    mov byte [gs:0x28],'A'
    mov byte [gs:0x29],0xA4

    mov byte [gs:0x2a],'D'
    mov byte [gs:0x2b],0xA4

    mov byte [gs:0x2c],'E'
    mov byte [gs:0x2d],0xA4

    mov byte [gs:0x2e],'R'
    mov byte [gs:0x2f],0xA4

    mov sp, LOADER_BASE_ADDR
    mov bp, loadermsg
    mov cx,17
    mov ax,0x1301
    mov bx,0x001f
    mov dx,0x1800
    int 0x10

    ;打开A20
    in al,0x92
    or al,0000_0010B
    out 0x92,al
    ;加载GDT
    lgdt [gdt_ptr]
    ;cr0第0位置1
    mov eax,cr0
    or eax,0x00000001
    mov cr0,eax

    jmp SELECTOR_CODE:p_mode_start

[bits 32]
p_mode_start:
    mov ax,SELECTOR_DATA
    mov ds,ax
    mov es,ax
    mov ss,ax
    mov esp,LOADER_STACK_TOP
    mov ax,SELECTOR_DATA
    mov gs,ax

    mov byte [gs:320], 'p'
    jmp $
