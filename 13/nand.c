#define NFCONF			(*(volatile unsigned long*)0x70200000)
#define NFCONT			(*(volatile unsigned long*)0x70200004)
#define NFCMMD			(*(volatile unsigned long*)0x70200008)
#define NFSTAT			(*(volatile unsigned long*)0x70200028)
#define NFADDR			(*(volatile unsigned long*)0x7020000C)
#define NFDATA			(*(volatile unsigned long*)0x70200010)
void select_chip(void)
{
	NFCONT &=~(1<<1); 
}
void disselect_chip(void)
{
	NFCONT |=(1<<1); 
}
void clean_RnB(void)
{
	NFSTAT |=(1<<4);
}

void nand_cmd(unsigned char cmd)
{
	NFCMMD=cmd;
}

void wait_RnB(void)
{
	while(!(NFSTAT&0x01));
}

void nand_addr(unsigned char addr)
{
	NFADDR=addr;
}

void nand_reset(void)
{
	//选中芯片
	select_chip();
	//清除RnB
	clean_RnB();
	//发出复位信号
	nand_cmd(0xff);
	//等待就绪
	wait_RnB();
	//取消选中
	disselect_chip();
}

void nand_init(void)
{
	//设置时序参数
#define TACLS 	7 
#define TWRPH0 	7 
#define TWRPH1	7 
	NFCONF &= ~((7<<12)|(7<<8)|(7<<4));
	 
}
