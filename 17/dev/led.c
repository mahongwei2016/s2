#ifdef mmu_on
#define GPKCON (volatile unsigned long*)0xa0008800
#define GPKDAT (volatile unsigned long*)0xa0008808
#else
#define GPKCON (volatile unsigned long*)0x7f008800
#define GPKDAT (volatile unsigned long*)0x7f008808
#endif
void led_on4(void)
{
    *(GPKCON) = 0x11110000;
    *(GPKDAT) = 0x70;
}
void led_on1(void)
{
    __asm__(
	"ldr r1, =0x11110000\n"
	"str r1, [%0]\n"
	"ldr r1, =0x00e0\n"
	"str r1, [%1]\n"
	:
    :"r"(GPKCON),"r"(GPKDAT)
	:"r1"
     );
}
void led_on2(void)
{
    *(GPKCON) = 0x11110000;
    *(GPKDAT) = 0xd0;
}
void led_on3(void)
{
    *(GPKCON) = 0x11110000;
    *(GPKDAT) = 0xb0;
}
