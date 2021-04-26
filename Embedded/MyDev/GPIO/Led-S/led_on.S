@ D10 - LED1 - GPF4
@ D11 - LED2 - GPF5
@ D12 - LED4 - GPF6
@ GPFCON -- 0x56000050
@ GPFDAT -- 0x56000054
@ GPF4 -- bits[9:8] -- 01
@ GPF5 -- bits[11:10] -- 01
@ GPF6 -- bits[13:12] -- 01
@ GPFCON = 0x00001500 (0...01010100000000)
.text
.global _start
_start:
    LDR R0 ,= 0x56000050
    MOV R1 ,# 0x00001500
    STR R1 , [R0]

    LDR R0 ,= 0x56000054
    MOV R1 ,# 0x00000020
    STR R1 , [R0]

MAIN_LOOP:
    B   MAIN_LOOP
