#define NFCONF			(*((volatile unsigned long*)0x70200000))
#define NFCONT			(*((volatile unsigned long*)0x70200004))
#define NFCMMD			(*((volatile unsigned char*)0x70200008))
#define NFSTAT			(*((volatile unsigned char*)0x70200028))
#define NFADDR			(*((volatile unsigned char*)0x7020000C))
#define NFDATA			(*((volatile unsigned char*)0x70200010))
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
	NFCONF |= (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4);
	//使能nandflash controller
	NFCONT =1|(1<<1); 
	//复位
	nand_reset(); 
}

void NF_PageRead(unsigned long addr,unsigned char* buff)
{
	int i;
	//选择芯片
	select_chip(); 
	//清除RnB
	clean_RnB();
	//发出命令0x00
	nand_cmd(0x00);
	//发出列地址
	nand_addr(0x00);
	nand_addr(0x00);
	//发出行地址
	nand_addr(addr&0xff);
	nand_addr((addr>>8)&0xff);
	nand_addr((addr>>16)&0xff);
	//发出命令0x30
	nand_cmd(0x30);
	//等待就绪
	wait_RnB(); 
	//读数据
	for(i=0;i<2048;i++)
		*buff++=NFDATA;
	//取消片选 
	disselect_chip();
}

void nand_to_ram(unsigned long start_addr,unsigned char* sdram_addr,int size)
{
	int i=0;
	for((i=(start_addr>>11));size>0;)
	{
		NF_PageRead(i,sdram_addr);
		size-=2048;
		sdram_addr+=2048;
		i++;
	}
}
/* 
int NF_Erase(unsigned long addr)
{
	int ret;
	//选择flash芯片
	select_chip(); 
	//清除RnB
	clean_RnB();
	//发送命令0x60
	nand_cmd(0x60);
	//发送行地址（3个周期）
	nand_addr(addr&0xff);
	nand_addr((addr>>8)&0xff);
	nand_addr((addr>>16)&0xff);
	//发送命令0xd0
	nand_cmd(0xd0);
	//等待RnB
	wait_RnB(); 
	//发送命令0x70
	nand_cmd(0x70); 
	//读取擦除结果
	ret=NFDATA;
	//取消选中flash芯片 
	disselect_chip();
}
 
void NF_WritePage(unsigned long addr,unsigned char *buff)
{
	int ret,i;
	//选择flash芯片
	select_chip(); 
	//清除RnB
	clean_RnB();
	//发送命令0x80
	nand_cmd(0x80);
	//发送列地址（2个周期）
	 nand_addr(0x00);
	 nand_addr(0x00);
	//发送行地址（3个周期）
	nand_addr(addr&0xff);
	nand_addr((addr>>8)&0xff);
	nand_addr((addr>>16)&0xff);
	//写入数据
	for(i=0;i<2048;i++)
		NFDATA=buff[i]; 
	//发送命令0x10
	nand_cmd(0x10);
	//等待RnB
	wait_RnB(); 
	//发送命令0x70
	nand_cmd(0x70); 
	//读取擦除结果
	ret=NFDATA;
	//取消选中flash芯片 
	disselect_chip();
}*/
int NF_Erase(unsigned long addr)
{
	int ret;
	
	//选中flash芯片
	select_chip();
	
	//清除RnB
	clean_RnB();
	
	//发送命令60
	nand_cmd(0x60);
	
	//发送行地址（3个周期）
	nand_addr(addr&0xff);
    	nand_addr((addr >>8 ) & (0xff));
    	nand_addr((addr >>16 ) & (0xff));
	
	//发送命令D0
	nand_cmd(0xD0);
	
	//等待RnB
	wait_RnB();
	
	//发送命令70
	nand_cmd(0x70);
	
	//读取擦除结果
	ret = NFDATA;
	
	//取消选中flash芯片
	disselect_chip();
	
	return ret;
}

int NF_WritePage(unsigned long addr,unsigned char* buff)
{
	int ret,i;
	
	//选中flash芯片
	select_chip();
	
	//清除RnB
	clean_RnB();
	
	//发送命令80
	nand_cmd(0x80);
	
	//发送列地址（2个周期）
	nand_addr(0x00);
    	nand_addr(0x00);
	
	//发送行地址（3个周期）
	nand_addr(addr&0xff);
    	nand_addr((addr >>8 ) & (0xff));
    	nand_addr((addr >>16 ) & (0xff));
	
	//写入数据
	for(i=0;i<1024*2;i++)
	{
		NFDATA = buff[i];	
	}
	
	//发送命令10
	nand_cmd(0x10);
	
	//等待RnB
	wait_RnB();
	
	//发送命令70
	nand_cmd(0x70);
	
	//读取写入结果
	ret = NFDATA;
	
	//取消选中flash芯片
	disselect_chip();
	
	return ret;
}


 
