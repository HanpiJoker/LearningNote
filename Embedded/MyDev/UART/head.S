@******************************************************
@File:head.S
@功能:设置SDRAM, 将程序复制到SDRAM, 然后跳到SDRAM继续执行
@******************************************************

.extern     main
.text
.global _start
_start:
Reset:
    ldr sp, =4096     @ 设置栈指针, 以下都是Ｃ函数, 调用前需要设好栈
    bl disable_watch_dog     @ 关闭WATCHDOG, 否则CPU会不断重启

@    ldr pc, =disable_watch_dog

    bl clock_init;
    bl memsetup
    bl copy_steppingstone_to_sdram
    ldr pc, =on_sdram

on_sdram:
    ldr sp, =0x34000000;
    ldr lr, =halt_loop
    ldr pc, =main

halt_loop:
    b halt_loop
