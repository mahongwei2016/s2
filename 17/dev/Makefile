objs := button.o interrupt.o led.o mmu.o nand.o uart.o dma.o lcd.o bmp.o dm9000.o arp.o

all : $(objs)
	arm-linux-ld -r -o dev.o $^
	
%.o : %.c
	arm-linux-gcc ${CFLAGS} -g -c $^
	
%.o : %.S
	arm-linux-gcc ${CFLAGS} -g -c $^

clean:
	rm -f *.o		
	
