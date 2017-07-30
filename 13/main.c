
volatile unsigned char buff[2048]={0};
#define nand_page_to_write 64*20+1
void nand_test(void)
{
	NF_Erase(nand_page_to_write); 
    buff[0]=100;
    NF_WritePage(nand_page_to_write,buff);
	buff[0]=10; 
	NF_PageRead(nand_page_to_write,buff);
	if(buff[0]==100)
		led_on4();
}
int gboot_main()
{
	#ifdef mmu_on
	mmu_enable();
	#endif
    led_on1();
   	nand_test();
    button_init();
	init_irq();
	while(1);
    return 0;    
}
