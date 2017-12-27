#define GPACON 	(*((volatile unsigned long*)0x7F008000))
#define ULCON0 	(*((volatile unsigned long*)0x7F005000))
#define UCON0 	(*((volatile unsigned long*)0x7F005004))
#define UMCON0 	(*((volatile unsigned long*)0x7F00500C))
#define UBRDIV0 	(*((volatile unsigned short*)0x7F005028))
#define UDIVSLOT0 	(*((volatile unsigned short*)0x7F00502C))
#define UTRSTAT0 	(*((volatile unsigned long*)0x7F005010))
#define UTXH0 	(*((volatile unsigned char*)0x7F005020))
#define URXH0 	(*((volatile unsigned char*)0x7F005024))
#define UFCON0     (*((volatile unsigned long *)0x7F005008))
#define UINTP0      (*((volatile unsigned long *)0x7F005030))
#define UINTSP0      (*((volatile unsigned long *)0x7F005034))
#define UINTM0      (*((volatile unsigned long *)0x7F005038))

#define VIC1INTENABLE (volatile unsigned long*)0x71300010
#define VIC1INTENCLEAR (volatile unsigned long*)0x71300014
#define VIC1VECTADDR5  (*(volatile unsigned long*)(0x71300114))
#define VIC0ADDRESS        (*((volatile unsigned long *)0x71200f00))
#define VIC1ADDRESS        (*((volatile unsigned long *)0x71300f00))
#define VIC1IRQSTATUS (*(volatile unsigned long*)0x71300000)
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
	UFCON0 = 0x07 | (1<<6); /* FIFO ENABLE */
	UCON0  = 0x5 | (1<<9);
	//设置波特率 
	UBRDIV0=(int)(PCLK/(BAUD*16)-1);
	UDIVSLOT0=0x0080;
}
void putc(unsigned char ch)
{
	int i=0;
	while(!(UTRSTAT0&(1<<2)))
		i++;
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
	putc(0x0d);
	putc(0x0a);
} 


void uart_irq(void)
{
	__asm__(
	"sub lr, lr, #4\n"
	"stmfd sp!, {r0-r12,lr}\n"
	:
	:
	);
	unsigned char c;
	*(VIC1INTENCLEAR)|=(1<<5);
	if(UINTP0 & (1<<2))
	{
		printf("uart handle:send\n\r");
	}else if(UINTP0 & (1<<0))
	{
		c=URXH0;
		printf("uart handle:reciver:%c\n\r",c);
	}
	UINTP0=0xf;
	UINTSP0=0xf;
	//printf("usp:%d\tup:%d,inter:%x,interenable:%x,interaddr:%x\n\r", UINTSP0, UINTP0,\
			VIC1IRQSTATUS, *VIC1INTENABLE, (VIC1ADDRESS));
	VIC0ADDRESS=0;
	VIC1ADDRESS=0;
	*(VIC1INTENABLE)|=(1<<5);
	__asm__(
	"ldmfd sp!, {r0-r12,pc}^ \n"
	:
	:
	);
}

void uart_irqinit(void)
{
	UINTM0|=(1<<2);
	UINTP0=0xf;
	(*(volatile unsigned int*)(0x71300114))=(unsigned int)uart_irq;
	*(VIC1INTENABLE)|=(1<<5);
}


void sleep(unsigned char s)
{
	unsigned char i=100,j=0;
	while(s--)
	{
		while(i--)
			j++;
	}
}
void uart_while(void)
{
	while(1)
	{
		unsigned char usp=UINTSP0;
		unsigned char up=UINTP0;
		//printf("usp:%d\tup:%d,inter:%x,interenable:%x,interaddr:%x\n\r",usp,up,VIC1IRQSTATUS,*VIC1INTENABLE,(VIC1ADDRESS));
		sleep(255);
	}
}

