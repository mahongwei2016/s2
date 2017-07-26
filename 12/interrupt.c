#define EINT0CON0 (volatile unsigned long*)0x7F008900
#define EINT0MASK (volatile unsigned long*)0x7F008920
#define VIC0INTENABLE (volatile unsigned long*)0x71200010
#define VIC0VECTADDR0 (volatile unsigned long*)0x71200100

#define EINT0PEND (volatile unsigned long*)0x7F008924
#define VIC0ADDRESS (volatile unsigned long*)0x71200F00
void key1_handle(void)
{
	__asm__(
	"sub lr, lr, #4\n"
	"stmfd sp!, {r0-r12,lr}\n"
	:
	:
	);
	if(*(EINT0PEND)==0x01)
		led_on2();
	else if(*(EINT0PEND)==0x02)
		led_on3();
	else
		led_on4();
	*(EINT0PEND)=0x03;
	*(VIC0ADDRESS)=0;
	__asm__(
	"sub lr, lr, #4\n"
	"ldmfd sp!, {r0-r12,pc}^\n"
	:
	:
	);
}

void init_irq(void)
{
	*(EINT0CON0)=0x02;
	*(EINT0MASK)&=0b00;
	*(VIC0INTENABLE)|=0x01;
	*(VIC0VECTADDR0)=(int)key1_handle;
	__asm__(
	"mrc p15, 0, r0, c1, c0, 0\n"
	"orr r0, r0, #(1<<24)\n"
	"mcr p15, 0, r0, c1, c0, 0\n"
	
	"mrs r0, cpsr\n"
	"bic r0, r0, #(1<<7)\n"
	"msr cpsr, r0\n"
	:
	:
	);
}