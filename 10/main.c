#define GPKCON (volatile unsigned long*)0x7f008800
#define GPKDAT (volatile unsigned long*)0x7f008808
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
int gboot_main()
{
    led_on2();
    
    return 0;    
}
