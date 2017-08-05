#define GPACON 	(*((volatile unsigned long*)0x7F008000))
#define ULCON0 	(*((volatile unsigned long*)0x7F005000))
#define UCON0 	(*((volatile unsigned long*)0x7F005004))
#define UMCON0 	(*((volatile unsigned long*)0x7F00500C))
#define UBRDIV0 	(*((volatile unsigned long*)0x7F005028))
#define UDIVSLOT0 	(*((volatile unsigned long*)0x7F00502C))
#define UTRSTAT0 	(*((volatile unsigned long*)0x7F005010))
#define UTXH0 	(*((volatile unsigned long*)0x7F005020))
#define URXH0 	(*((volatile unsigned long*)0x7F005024))
#define UFCON0     (*((volatile unsigned long *)0x7F005008))
#define PCLK 66500000
#define BAUD 115200
void uart_init()
{
	//配置引进功能
	GPACON&=~(0xff);
	GPACON|=0x22;
	//设置数据格式 
	ULCON0=0b11;
	//设置工作模式
	UFCON0 = 0x01; /* FIFO ENABLE */
	UCON0=0x05;
	//设置波特率 
	UBRDIV0=(int)(PCLK/(BAUD*16)-1);
	UDIVSLOT0=0x0080;
}
void putc(unsigned char ch)
{
	while(!(UTRSTAT0&(1<<2)));
	UTXH0=ch;
}
unsigned char getc(void)
{
	unsigned char ret;
	while(!(UTRSTAT0&(1<<0)));
	ret=URXH0;
	if ( (ret == 0x0d) || (ret == 0x0a) )
	{
	    putc(0x0d);
	    putc(0x0a);	
	}      	
	else
	    putc(ret);
	    
        return ret;
}
void uart_send_string(char *str)
{
	while(*str)
		putc(*str++);
	//putc(0x0d);
	//putc(0x0a);
} 

#define VIC1INTENABLE (volatile unsigned long*)0x71300010
#define VIC1VECTADDR5 (volatile unsigned long*)0x71300114
#define VIC1ADDRESS (volatile unsigned long*)0x71300F00
void uart_handle(void)
{
	__asm__(
	//保护环境，因为流水线，pc+12，lr+8
	"sub lr, lr, #4\n"
	"stmfd sp!, {r0-r12,lr}\n"
	:
	:
	);
	printf("uart handle\n");
	*(VIC1ADDRESS)=0;
	__asm__(
	"sub lr, lr, #4\n"
	//^意思是把SPSR拷贝到CPSR
	"ldmfd sp!, {r0-r12,pc}^\n"
	:
	:
	);
}

void uart_irqinit(void)
{
	UMCON0|=(1<<3); 
	*(VIC1INTENABLE)|=(1<<6);
	*(VIC1VECTADDR5)=(int)uart_handle;
}

