led_on_c.bin : crt0.S main.c
	arm-linux-gcc -g -c -o crt0.o crt0.S
	arm-linux-gcc -g -c -o main.o main.c
	arm-linux-ld  -Ttext 0x00000000 -g crt0.o main.o -o led_on_c_elf
	arm-linux-objcopy -O binary -S led_on_c_elf led_on_c.bin
	arm-linux-objdump -D -m arm led_on_c_elf > led_on_c.dis

clean :
	rm -f led_on_c.bin led_on_c.dis led_on_c_elf *.o