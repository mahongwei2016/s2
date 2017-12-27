

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
	timer_init();
	int i=0;
	while(1)
	{
		i++;
		printf("i:%d\n\r",i);
	}
	lcd_init();
	printf("lcd test1\r\n");
	lcd_clear_screen(300);
	printf("lcd test\r\n");
	//lcd_test(); 
	dm9000_init();
	while(1)
	{
		static int j=0;
		j++;
		arp_request(j);
		delay(10000);
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