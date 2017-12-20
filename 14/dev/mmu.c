/*
* 用于段描述符的一些宏定义 
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
	
	/*设置TTB*/
	"ldr r0, =0x50000000\n"
	"mcr p15, 0, r0, c2, c0, 0\n" 
	
	/*不进行权限检查*/
	"mvn r0,#0\n"
	"mcr p15, 0, r0, c3, c0, 0\n" 
	
	/*使能mmu*/
	"mrc p15, 0, r0, c1, c0, 0\n"
	"orr r0, r0, #0x01\n"
	"mcr p15, 0, r0, c1, c0, 0\n"
	:
	: 
	);
}
void mmu_enable(void)
{
	create_page_table();
	mmu_init();
}