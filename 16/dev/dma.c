/*#define SDMA_SEL (*((volatile unsigned long*)0x7E00F110))
#define DMACSync (*((volatile unsigned long*)0x7E000034))
#define DMACConfiguration (*((volatile unsigned long*)0x7E000030))
#define DMACC0SRCADDR (*((volatile unsigned long*)0x7E000100))
#define DMACC0DestAddr (*((volatile unsigned long*)0x7E000104))
#define UTXH0 	(volatile unsigned long*)0x7F005020
#define DMACC0Control0 (*((volatile unsigned long*)0x7E00010c))
#define DMACC0Control1 (*((volatile unsigned long*)0x7E000110))
#define DMACC0Configuration (*((volatile unsigned long*)0x7E000114))*/
#define SDMA_SEL      		(*((volatile unsigned long *)0x7E00F110))
#define DMACIntTCClear      	(*((volatile unsigned long *)0x7DB00008))
#define DMACIntErrClr      	(*((volatile unsigned long *)0x7DB00010))
#define DMACConfiguration      	(*((volatile unsigned long *)0x7DB00030))
#define DMACSync      		(*((volatile unsigned long *)0x7DB00034))
#define DMACC0SRCADDR     	(*((volatile unsigned long *)0x7DB00100))
#define DMACC0DestAddr     	(*((volatile unsigned long *)0x7DB00104))
#define DMACC0Control0      	(*((volatile unsigned long *)0x7DB0010c))
#define DMACC0Control1      	(*((volatile unsigned long *)0x7DB00110))
#define DMACC0Configuration     (*((volatile unsigned long *)0x7DB00114))
#define UTXH0      0x7F005020
char srcmhw[100]=" I love xuxu,too\n\r";
void dma_init(void)
{
	//DMA控制器的选择（SMDAC0）
	SDMA_SEL=0;
	//如果外设的工作始终与DMA控制器的始终不相同，要使能同步逻辑
	DMACSync =0;
	//DMA控制器使能 
	DMACConfiguration=1;
	//初始化源地址
	DMACC0SRCADDR=(unsigned int)srcmhw; 
	//初始化目的地址
	DMACC0DestAddr= (unsigned int)UTXH0;
	//对控制器寄存器进行设置
	DMACC0Control0=(1<<26)|(1<<25)|(1<<31);
	DMACC0Control1=0x64;//传输大小 
	//开启channel0 DMA
	/*
	流控制和传输类型：MTP为001
	目标外设：DMA_UART0_1，源外设：DMA_MEM
	通道有效：1 
	*/ 
	DMACC0Configuration=(1<<15)|(1<<14)|(1<<11)|(1<<6);
	
}
void dma_start()
{
	DMACC0Configuration=1; 
}


