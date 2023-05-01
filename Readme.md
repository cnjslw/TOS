## Debugs Log

### 编译报错：undefined reference to “__stack_chk_fail” 

编译源码到目标文件时，一定要加“-fno-stack-protector”，不然默认会调函数“__stack_chk_fail” 进行栈相关检查，然而是手动裸ld去链接，没有链接到“__stack_chk_fail”所在库文件，所以在链接过程一定会报错: undefined reference to “__stack_chk_fail” 。解决办法不是在链接过程中，而是在编译时加此参数，强制gcc不进行栈检查，从而解决。此外，ld 的时候加上参数"-e main"就可以了，意思是将main函数作为程序入口，ld 的默认程序入口为_start。即：gcc -c a.c -fno-stack-protector -o a.old a.o b.o -e main -o ab

### 编译报错：number of operands mismatch for push Error: invalid instruction suffix for pop

64位gcc编译生成的32位汇编程序运行就会报错，所以需要让gcc兼容32位汇编，首先在编译的时候加上-m32参数选项
