#define GPNCON (volatile unsigned long*)0x7f008830
void button_init(void)
{
	*(GPNCON)=0x0a;
}