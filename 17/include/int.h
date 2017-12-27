void intc_init(void);
void intc_enable(unsigned long intnum);
void intc_disable(unsigned long intnum);
void intc_setvectaddr(unsigned long intnum, void (*handler)(void));
void intc_clearvectaddr(void);
unsigned long intc_getvicirqstatus();

//INT NUM - VIC0
#define NUM_EINT0				(0)
#define NUM_EINT1				(1)
#define NUM_TIMER0				(23)
#define NUM_TIMER3				(27)
#define NUM_ADC					(63)

