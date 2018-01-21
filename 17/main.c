#include "s3c6410.h"
extern unsigned char *gImage_bmp;
extern int gLCD_Type;
#define nand_page_to_write 64*20+1
void delay(char i)
{
	char j=0;
	while(i--)
		while(j--);
}
void nand_test(void)
{
	volatile unsigned char buff[2048]={0};
	NF_Erase(nand_page_to_write); 
    buff[0]=100;
    NF_WritePage(nand_page_to_write,buff);
	buff[0]=10; 
	NF_PageRead(nand_page_to_write,buff);
	if(buff[0]==100)
		led_on4();
}
char *buf="I love linux\r\n";

int gboot_main()
{
	int num;
	#ifdef mmu_on
	mmu_enable();
	#endif
    led_on1();
   	nand_test();
    button_init();
	init_irq();
	uart_init();
	putc('A');
	printf("B\r\n");
	uart_send_string(buf);
	dma_init();
	dma_start();
	/*timer_init();
	while(1)
	{
		int i=0;
		i++;
		//printf("i:%d\n\r",i);
	}*/
	lcd_init();
	Lcd_Init(gLCD_Type);
	delay(200);
	printf("lcd clear!\r\n");
	lcd_clear_screen(0xffff00);
	Lcd_Draw_Bmp(gImage_bmp,gLCD_Type);
	One_Wire_Timer_Proc();
	while(1);
	delay(200);
	dm9000_init();
	while(1)
	{
		static int j=0;
		j++;
		arp_request(j);
		delay(100);
	}
	while(1)
	{
		printf("*******************************************\n\r");
		printf("********************GBOOT******************\n\r");
		printf("1:Download Linux Kernel from TFTP Server��\n\r");
		printf("2:Boot Linux from RAM!\n\r");
		printf("3:Boot Linux from Nand Flash\n\r");
		printf("\n Please Select:"); 
		scanf("%d",&num);
		printf("%d\n",num);
		switch(num)
		{
			case 1: 
				break;
			case 2:
				break;
			case 3:
				break;
			default:
				printf("Error:wrong selection!\n\r");
				break;	
		}	
	}
    return 0;    
}
