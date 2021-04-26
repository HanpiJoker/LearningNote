@ File: crt0.S
@ Function: Jump to C program;
@ WatchDog: WTCON -- 0x53000000

.text
.global _start
_start:
    ldr r0 ,= 0x53000000
    mov r1 ,# 0x0
    str r1 ,  [r0]  @ 在WatchDog中写入0, 否则cpu会不断重启
    
    ldr sp , = 1024*4 @ 设置堆栈, 不能超过4KB, 目前可用内存4KB

    bl  main

halt_loop:
    b  halt_loop
