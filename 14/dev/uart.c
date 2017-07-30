#define GPACON 	(*((volatile unsigned long*)0x7F008000))
#define ULCON0 	(*((volatile unsigned long*)0x7F005000))
#define UCON0 	(*((volatile unsigned long*)0x7F005004))
#define UBRDIV0 	(*((volatile unsigned long*)0x7F005028))
#define UDIVSLOT0 	(*((volatile unsigned long*)0x7F00502C))
#define UTRSTAT0 	(*((volatile unsigned long*)0x7F005010))
#define UTXH0 	(*((volatile unsigned long*)0x7F005020))
#define URXH0 	(*((volatile unsigned long*)0x7F005024))
#define PCLK 66500000
#define BAUD 115200
void uart_init()
{
	//配置引进功能
	GPACON&=~(0xff);
	GPACON|=0x22;
	//设置数据格式 
	ULCON0=0b111;
	//设置工作模式
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
