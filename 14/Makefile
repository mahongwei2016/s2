CFLAGS := -fno-builtin -I$(shell pwd)/include
export CFLAGS
all: start.o mem.o dev/dev.o lib/lib.o main.o
	arm-linux-ld -Tgboot.lds -o gboot.elf $^
	arm-linux-objcopy -O binary gboot.elf gboot.bin
	cp gboot.bin ../../
	
%.o : %.S
	arm-linux-gcc -g -c $^
	
%.o : %.c
	arm-linux-gcc $(CFLAGS)-g -c $^
	
lib/lib.o:
	make -C lib all

dev/dev.o:
	make -C dev all
	
.PHONY: clean
clean:
	make -C lib clean
	make -C dev clean
	rm *.o *.elf *.bin


