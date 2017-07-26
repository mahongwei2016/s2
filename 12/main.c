

int gboot_main()
{
	#ifdef mmu_on
	mmu_enable();
	#endif
    led_on1();
    button_init();
	init_irq();
	while(1);
    return 0;    
}
