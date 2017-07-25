#define GPKCON (volatile unsigned long*)0xa0008800
#define GPKDAT (volatile unsigned long*)0xa0008808
void led_on1(void)
{
    *(GPKCON) = 0x11110000;
    *(GPKDAT) = 0xa0;
}
void led_on2(void)
{
    __asm__(
	"ldr r1, =0x11110000\n"
	"str r1, [%0]\n"
	"ldr r1, =0x00a0\n"
	"str r1, [%1]\n"
	:
        :"r"(GPKCON),"r"(GPKDAT)
	:"r1"
         );
}
/*
* ���ڶ���������һЩ�궨�� 
*/
#define MMU_SECTION (2)
#define MMU_BUFFERABLE (1<<2)
#define MMU_CACHEABLE (1<<3)
#define MMU_SPECIAL		(1<<4)
#define MMU_DOMAIN		(0<<5)
#define MMU_FULL_ACCESS	(3<<10)
#define MMU_SECDESC		(MMU_FULL_ACCESS)|(MMU_DOMAIN)|(MMU_SPECIAL)|(MMU_CACHEABLE)|(MMU_BUFFERABLE)|(MMU_SECTION)
void create_page_table(void)
{
	unsigned int *ttb=(unsigned int*)0x50000000;
	unsigned int vaddr,paddr;
	vaddr=0xa0000000;
	paddr=0x7f000000;
	*(ttb+(vaddr>>20))=(paddr&0xfff00000)|MMU_SECDESC;
	
	vaddr=0x50000000;
	paddr=0x50000000;
	while(paddr<0x54000000)
	{
		*(ttb+(vaddr>>20))=(paddr&0xfff00000)|MMU_SECDESC;	
		vaddr+=0x100000;
		paddr+=0x100000;
	}
}
void mmu_init(void)
{
	__asm__(
	
	/*����TTB*/
	"ldr r0, =0x50000000\n"
	"mcr p15, 0, r0, c2, c0, 0\n" 
	
	/*������Ȩ�޼��*/
	"mvn r0,#0\n"
	"mcr p15, 0, r0, c3, c0, 0\n" 
	
	/*ʹ��mmu*/
	"mrc p15, 0, r0, c1, c0, 0\n"
	"orr r0, r0, #0x01\n"
	"mcr p15, 0, r0, c1, c0, 0\n"
	:
	: 
	);
}
int gboot_main()
{
	create_page_table();
	mmu_init();
    led_on2();
    
    return 0;    
}
