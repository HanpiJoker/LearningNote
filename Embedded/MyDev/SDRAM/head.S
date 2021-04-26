
@******************************************************************************
@File:head.S
@功能：设置SDRAM，将程序复制到啊SDRAM，然后跳到SDRAM继续执行
@******************************************************************************

@ .equ -- 用于把常量值设置为可以在文本段中使用的符号, (类似于define)
.equ    MEM_CTL_BASE,       0x48000000
.equ    SDRAM_BASE,         0x30000000


.text
.global _start
_start:
    bl  disable_watch_dog                    @关闭WATCHDOG，否则CPU会不断重启
    bl  memsetup                             @设置存储控制器
    bl  copy_steppingstone_to_sdram          @复制代码到SDRAM中
    ldr pc ,= on_sdram                       @跳到SDRAM中继续执行
on_sdram:
    ldr sp ,= 0x34000000                     @设置栈
    bl  main

halt_loop:
    b   halt_loop

disable_watch_dog:
    @往WATCHDOG寄存器写0即可
    mov r1 ,# 0x53000000
    mov r2 ,# 0x0
    str r2 ,[r1]
    mov pc , lr                             @ 返回主函数

copy_steppingstone_to_sdram:
    @将steppingstone的4KB数据全部复制到SDRAM中去
    @Steppingstone起始地址为0x00000000，SDRAM中的起始地址为0x30000000
    
    mov r1 ,# 0
    ldr r2 ,= SDRAM_BASE
    mov r3 ,# 4*1024
1:
    ldr r4 ,[r1] ,# 4                        @从Steppingstone读取4字节的数据，并让原地址加4
    str r4 ,[r2] ,# 4                        @将此4字节的数据复制到SDRAM中，并让目的地址加4
    cmp r1 , r3                              @判断是否完成：原地址等于Steppingstone的，末地址？ 
    bne 1b                                   @若没有复制完，继续
    mov pc ,    lr                           @返回 

memsetup:
    @设置存储控制器以便使用SDRAM等外设

    mov r1 ,# MEM_CTL_BASE                   @ 存储控制器13个寄存器的开始地址
    adr r2 , mem_cfg_val                    @ 这13个值的起始存储地址
    add r3 , r1 , #52                        @ 13*4=52
1:
    ldr r4, [r2], #4                         @ 读取设置值，并让r2加4
    str r4, [r1], #4                         @ 将此值写入寄存器，并让r1加4
    cmp r1, r3
    bne 1b
    mov pc, lr 

.align 4
mem_cfg_val:
    .long 0x22011110
    .long 0x00000700
    .long 0x00000700
    .long 0x00000700
    .long 0x00000700
    .long 0x00000700
    .long 0x00000700
    .long 0x00018005
    .long 0x00018005
    .long 0x008C07A3
    .long 0x000000B1
    .long 0x00000030
    .long 0x00000030
