#include "arp.h"
#define SROM_BW  (*((volatile unsigned long *)0X70000000))
#define SROM_BC1  (*((volatile unsigned long *)0X70000008))
#define GPNCON (*((volatile unsigned long *)0x7f008830))
#define EINT0CON0 (*((volatile unsigned long *)0x7F008900))
#define EINT0MASK (*((volatile unsigned long *)0x7F008920))
#define EINT0PEND (*((volatile unsigned long *)0x7F008924))
#define VIC0INTENCLEAR (*((volatile unsigned long *)0x71200014))
#define VIC0INTENABLE (*((volatile unsigned long *)0x71200010))
#define VIC0VECTADDR (*((volatile unsigned long *)0x71200104))
#define VIC0ADDRESS (*((volatile unsigned long*)0x71200F00))
#define VIC1ADDRESS (*((volatile unsigned long*)0x71300F00))
void int_issur(void);
u8 host_mac_addr[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
u8 mac_addr[6] = {9,8,7,6,5,4};
u8 ip_addr[4] = {192,168,0,30};
u8 host_ip_addr[4] = {192,168,0,112};
u16 txpacket_len=0;

unsigned char txbuffer[1522];
unsigned char rxbuffer[1522];
void cs_init(void)
{
	SROM_BW&=(~(0xf<<4));
	SROM_BW|=(1<<4);
	SROM_BC1|=(0<<0)|(0x6<<4)|(0x4<<8)|(0x1<<12)|(0xe<<16)|(0x4<<24)|(0x0<<28);
} 

void int7_init(void)
{
	//配置int7为中断 
	GPNCON &=(~(3<<14));
	GPNCON |=(2<<14);
	//配置int7位 
	EINT0CON0 &=(~(7<<12)); 
	EINT0CON0 |=(1<<12);
	//外部中断使能
	EINT0MASK&=(~(1<<7));
	//清除下发生的int7
	EINT0PEND|=(1<<7);
	//int7输出中断源1，使能中断源1
	//VIC0INTENCLEAR|=(1<<1);
	VIC0INTENABLE|=(1<<1);
	//设置向量中断的地址
	VIC0VECTADDR=(int) int_issur;
	
} 

#define DM_ADD (*((volatile unsigned short *)0x18000300))
#define DM_DAT (*((volatile unsigned short *)0x18000304))

void dm9000_reg_write(unsigned int reg,unsigned int data)
{
	DM_ADD=reg;
	DM_DAT=data;
}

unsigned int dm9000_reg_read(unsigned int reg)
{
	DM_ADD=reg;
	return DM_DAT;
}

void dm9000_reset()
{
	printf("reset dm9000\n\r");
	/* DEBUG: Make all GPIO0 outputs, all others inputs */
	dm9000_reg_write(0x1e,1<<0);
	/* Step 1: Power internal PHY by writing 0 to GPIO0 pin */
	dm9000_reg_write(0x1f,0);
	/* Step 2: Software reset */
	dm9000_reg_write(0x00,3);
	do{
		printf("resetting the DM9000, 2nd reset\n\r");	
		int i=10000;
		while(i--);
	}while(dm9000_reg_read(0x00)&0x01);
	dm9000_reg_write(0x00,0);
	dm9000_reg_write(0x00,3);
	do{
		printf("resetting the DM9000, 2nd reset\n\r");	
		int i=10000;
		while(i--);
	}while(dm9000_reg_read(0x00)&0x01);
	dm9000_reg_write(0x00,0);
	/* Check whether the ethernet controller is present */
	if ((dm9000_reg_read(0x2a) != 0x0) ||
	    (dm9000_reg_read(0x2b) != 0x90))
		printf("ERROR: resetting DM9000 -> not responding\n");
	
}

int dm9000_probe(void)
{
	unsigned int id_val;
	id_val = dm9000_reg_read(0x28);
	id_val |= dm9000_reg_read(0x29) << 8;
	id_val |= dm9000_reg_read(0x2a) << 16;
	id_val |= dm9000_reg_read(0x2b) << 24;
	if (id_val == 0x90000A46) {
		printf("dm9000 i/o: 0x%x, id: 0x%x \n\r", 0x18000300,
		       id_val);
		return 0;
	} else {
		printf("dm9000 not found at 0x%08x id: 0x%08x\n\r",
		       0x18000300, id_val);
		return -1;
	}	
}

//unsigned char mac_addr[6]={6,5,4,3,2,1};
void dm9000_init()
{
	//设置片选
	cs_init();
	//中断初始化 
	int7_init();
	//复位设备
	dm9000_reset();
	//捕捉DM9000
	dm9000_probe();
	//MAC初始化
	/* Program operating register, only internal phy supported */
    dm9000_reg_write(0x0,0x0);
    /* TX Polling clear */
    dm9000_reg_write(0x02,0x0);
    /* Less 3Kb, 200us */
    dm9000_reg_write(0x08,0x3f);
    /* Flow Control : High/Low Water */
    dm9000_reg_write(0x09,0x38);
    /* SH FIXME: This looks strange! Flow Control */    
    dm9000_reg_write(0x0a,0);
	/* Special Mode */
    dm9000_reg_write(0x2f,0x0);
    /* clear TX status */
    dm9000_reg_write(0x01,1<<5|1<<3|1<<2);
    /* Clear interrupt status */
    dm9000_reg_write(0xfe,0xf);
	//填充MAC地址
	int i=0;
	for(i=0;i<6;i++)
	{
		dm9000_reg_write(0x10+i,mac_addr[i]);	
	}
	printf("eth_addr:");
    for(i=0;i<6;i++)
		printf("%x ",dm9000_reg_read(0x10+i));
    printf("\n\r");
	//激活DM9000
	dm9000_reg_write(0x05,1<<5|1<<4|1<<0);
    dm9000_reg_write(0xff,1<<7);  
}

unsigned int dm9000_tx(char* data,int length)
{
	//禁止中断
	EINT0MASK|=1<<7;
	dm9000_reg_write(0xff,0x80);
	//写入待发送数据的长度
	dm9000_reg_write(0xfc, length & 0xff);
	dm9000_reg_write(0xfd, (length >> 8) & 0xff);
	//写入待发送的数据
	DM_ADD=0xf8;
	int i=0;
	for(i=0;i<length;i+=2)
	{
		DM_DAT=data[i]|(data[i+1]<<8);	
	}
	//启动发生
	dm9000_reg_write(0x02,0x01);
	//等待发送结束
	while(1)
	{
        unsigned char status;
		status = dm9000_reg_read(0x02);
        if((status&0x01)==0x00)
		break;
		printf("sending!\r\n");
	}
	i=1000;
	while(i--);
	//printf("send over!");
	//清楚发送状态
	dm9000_reg_write(0x01,0x2c);
	//恢复中断使能 
	dm9000_reg_write(0xff,0x81);
	EINT0MASK&=(~(1<<7));
	return length;
}
 
int dm9000_rx(unsigned char *data)
{
	unsigned short status=0,len=0;
    unsigned int i=0;
    unsigned short tmp=0;
	//判断是否发出接收中断，并清除 
	 /*clear interrupt*/
    if(dm9000_reg_read(0xfe)&0x01)
		dm9000_reg_write(0xfe,0x01);
    else
		return 0;
	//空读 
	/* Dummy read */
	dm9000_reg_read(0xf0);
	//读取状态 
    status=dm9000_reg_read(0xf2);
    printf("staus:%x\n\r",status);
	//读取包长度 
	len=DM_DAT;
	printf("recieve %d byte\n\r",len);
	//读取数据 
	if(len<1522)
	{
		for(i=0;i<len;i+=2)
		{
		    tmp=DM_DAT;
		    data[i]=tmp&0xff;
		    data[i+1]=(tmp>>8)&0x0ff;
		#if 1
		    if(i==0)
	        {
	            if((data[0]!=9)&&(data[1]!=8))
					return 0;
	        }
		#endif
		}
	}
	else 
		{printf("recv eroor\r\n");
		return 0;}
	return len; 
}

void int_issur(void)
{    
    __asm__(
        "sub lr,lr,#4\n"
        "stmfd sp!,{r0-r12,lr}\n"
        :
        :
        );
    u16 rxpacket_len=0;
    EINT0MASK|=(1<<7);
    static int i;
    printf("interrupt:%d\n\r",i++);
    rxpacket_len=dm9000_rx(rxbuffer);
    printf("recv:%dbyte\n\r",rxpacket_len);
    if(rxpacket_len!=0)
		net_handle(rxbuffer,rxpacket_len);
	
    /*clear interrupt*/
    dm9000_reg_write(0xfe,0x01);

    EINT0PEND|=(1<<7);
    VIC0ADDRESS=0;
    VIC1ADDRESS=0;
    EINT0MASK&=(~(1<<7));
    __asm__(
        "ldmfd sp!,{r0-r12,pc}^\n"
        :
        :
        ); 
}    
