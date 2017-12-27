/************************************************
 * NAME	    : S5PV210.H
 * Version  : 31.3.2012
 ************************************************/
#ifndef __S5PV210_H__
#define __S5PV210_H__
#define pISR_TIMER0 ( *((volatile unsigned long *)(0x7120015C)) )
#define pISR_TIMER3 ( *((volatile unsigned long *)(0x7120016C)) )



// TIMER
#define		PWMTIMER_BASE			(0x7F006000)
#define		TCFG0    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x00)) )
#define		TCFG1    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x04)) )
#define		TCON      	( *((volatile unsigned long *)(PWMTIMER_BASE+0x08)) )
#define		TCNTB0    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x0C)) )
#define		TCMPB0    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x10)) )
#define		TCNTO0    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x14)) )
#define		TCNTB1    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x18)) )
#define		TCMPB1    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x1C)) )
#define		TCNTO1    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x20)) )
#define		TCNTB2    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x24)) )
#define		TCMPB2    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x28)) )
#define		TCNTO2    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x2C)) )
#define		TCNTB3    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x30)) )
#define		TCMPB3    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x34)) )
#define		TCNTO3    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x38)) )
#define		TCNTB4    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x3C)) )
#define		TCNTO4    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x40)) )
#define		TINT_CSTAT 	( *((volatile unsigned long *)(PWMTIMER_BASE+0x44)) )


// Interrupt
#define GPNCON     			(*((volatile unsigned long *)0x7F008830))
#define GPNDAT     			(*((volatile unsigned long *)0x7F008834))
#define EINT0CON0  			(*((volatile unsigned long *)0x7F008900))
#define EINT0MASK  			(*((volatile unsigned long *)0x7F008920))
#define EINT0PEND  			(*((volatile unsigned long *)0x7F008924))
#define PRIORITY    		(*((volatile unsigned long *)0x7F008280))
#define SERVICE     		(*((volatile unsigned long *)0x7F008284))
#define SERVICEPEND 		(*((volatile unsigned long *)0x7F008288))

#define VIC0IRQSTATUS  		(*((volatile unsigned long *)0x71200000))
#define VIC1IRQSTATUS  		(*((volatile unsigned long *)0x71300000))
#define VIC0FIQSTATUS  		(*((volatile unsigned long *)0x71200004))
#define VIC0RAWINTR    		(*((volatile unsigned long *)0x71200008))
#define VIC0INTSELECT  		(*((volatile unsigned long *)0x7120000c))
#define VIC1INTSELECT  		(*((volatile unsigned long *)0x7130000c))
#define VIC0INTENABLE  		(*((volatile unsigned long *)0x71200010))
#define VIC1INTENABLE  		(*((volatile unsigned long *)0x71300010))
#define VIC0INTENCLEAR 		(*((volatile unsigned long *)0x71200014))
#define VIC1INTENCLEAR 		(*((volatile unsigned long *)0x71300014))
#define VIC0PROTECTION 	   	(*((volatile unsigned long *)0x71200020))
#define VIC0SWPRIORITYMASK 	(*((volatile unsigned long *)0x71200024))
#define VIC0PRIORITYDAISY  	(*((volatile unsigned long *)0x71200028))
#define VIC0VECTADDR       	(*((volatile unsigned long *)0x71200100))
#define VIC1VECTADDR       	(*((volatile unsigned long *)0x71300100))
#define VIC0ADDRESS        	(*((volatile unsigned long *)0x71200f00))
#define VIC1ADDRESS        	(*((volatile unsigned long *)0x71300f00))


// UART相关寄存器
#define ULCON0     (*((volatile unsigned long *)0x7F005000))
#define UCON0      (*((volatile unsigned long *)0x7F005004))
#define UFCON0     (*((volatile unsigned long *)0x7F005008))
#define UMCON0     (*((volatile unsigned long *)0x7F00500C))
#define UTRSTAT0   (*((volatile unsigned long *)0x7F005010))
#define UFSTAT0    (*((volatile unsigned long *)0x7F005018))
#define UTXH0      (*((volatile unsigned char *)0x7F005020))
#define URXH0      (*((volatile unsigned char *)0x7F005024))
#define UBRDIV0    (*((volatile unsigned short *)0x7F005028))
#define UDIVSLOT0  (*((volatile unsigned short *)0x7F00502C))
#define GPACON     (*((volatile unsigned long *)0x7F008000))


// 时钟相关寄存器
#define APLL_LOCK (*((volatile unsigned long *)0x7E00F000))
#define MPLL_LOCK (*((volatile unsigned long *)0x7E00F004))
#define EPLL_LOCK (*((volatile unsigned long *)0x7E00F008))
#define OTHERS    (*((volatile unsigned long *)0x7e00f900))
#define CLK_DIV0  (*((volatile unsigned long *)0x7E00F020))
#define APLL_CON  (*((volatile unsigned long *)0x7E00F00C))
#define APLL_CON_VAL  ((1<<31) | (266 << 16) | (3 << 8) | (1))
#define MPLL_CON  (*((volatile unsigned long *)0x7E00F010))
#define MPLL_CON_VAL  ((1<<31) | (266 << 16) | (3 << 8) | (1))
#define CLK_SRC  (*((volatile unsigned long *)0x7E00F01C))


// RTC
#define 	RTC_BASE	 (0x7E005000)
#define		INTP      	 ( *((volatile unsigned long *)(RTC_BASE + 0x30)) )
#define		RTCCON    	 ( *((volatile unsigned long *)(RTC_BASE + 0x40)) )
#define		TICCNT    	 ( *((volatile unsigned long *)(RTC_BASE + 0x44)) )
#define		RTCALM    	 ( *((volatile unsigned long *)(RTC_BASE + 0x50)) )
#define		ALMSEC    	 ( *((volatile unsigned long *)(RTC_BASE + 0x54)) )
#define		ALMMIN    	 ( *((volatile unsigned long *)(RTC_BASE + 0x58)) )
#define		ALMHOUR  	 ( *((volatile unsigned long *)(RTC_BASE + 0x5c)) )
#define		ALMDATE    	 ( *((volatile unsigned long *)(RTC_BASE + 0x60)) )
#define		ALMMON    	 ( *((volatile unsigned long *)(RTC_BASE + 0x64)) )
#define		ALMYEAR  	 ( *((volatile unsigned long *)(RTC_BASE + 0x68)) )
#define		BCDSEC    	 ( *((volatile unsigned long *)(RTC_BASE + 0x70)) )
#define		BCDMIN   	 ( *((volatile unsigned long *)(RTC_BASE + 0x74)) )
#define		BCDHOUR    	 ( *((volatile unsigned long *)(RTC_BASE + 0x78)) )
#define		BCDDATE    	 ( *((volatile unsigned long *)(RTC_BASE + 0x7c)) )
#define		BCDDAY     	 ( *((volatile unsigned long *)(RTC_BASE + 0x80)) )
#define		BCDMON     	 ( *((volatile unsigned long *)(RTC_BASE + 0x84)) )
#define		BCDYEAR      ( *((volatile unsigned long *)(RTC_BASE + 0x88)) )
#define		CURTICCNT    ( *((volatile unsigned long *)(RTC_BASE + 0x90)) )

#define 	FROM_BCD(n)		 			((((n) >> 4) * 10) + ((n) & 0xf))
#define 	TO_BCD(n)		 			((((n) / 10) << 4) | ((n) % 10))

// LCD
#define GPECON  (*((volatile unsigned long *)0x7F008080))
#define GPEDAT  (*((volatile unsigned long *)0x7F008084))
#define GPEPUD  (*((volatile unsigned long *)0x7F008088))
#define GPFCON  (*((volatile unsigned long *)0x7F0080A0))
#define GPFDAT  (*((volatile unsigned long *)0x7F0080A4))
#define GPFPUD  (*((volatile unsigned long *)0x7F0080A8))
#define GPICON  (*((volatile unsigned long *)0x7F008100))
#define GPIPUD  (*((volatile unsigned long *)0x7F008108))
#define GPJCON  (*((volatile unsigned long *)0x7F008120))
#define GPJPUD  (*((volatile unsigned long *)0x7F008128))

/* display controller */
#define MIFPCON  	    (*((volatile unsigned long *)0x7410800C))
#define SPCON        	(*((volatile unsigned long *)0x7F0081A0))
#define VIDCON0      	(*((volatile unsigned long *)0x77100000))
#define VIDCON1      	(*((volatile unsigned long *)0x77100004))
#define VIDTCON0     	(*((volatile unsigned long *)0x77100010))
#define VIDTCON1     	(*((volatile unsigned long *)0x77100014))
#define VIDTCON2     	(*((volatile unsigned long *)0x77100018))
#define WINCON0      	(*((volatile unsigned long *)0x77100020))
#define VIDOSD0A      	(*((volatile unsigned long *)0x77100040))
#define VIDOSD0B      	(*((volatile unsigned long *)0x77100044))
#define VIDOSD0C      	(*((volatile unsigned long *)0x77100048))
#define VIDW00ADD0B0    (*((volatile unsigned long *)0x771000A0))
#define VIDW00ADD1B0    (*((volatile unsigned long *)0x771000D0))
#define VIDW00ADD2      (*((volatile unsigned long *)0x77100100))
#define DITHMODE        (*((volatile unsigned long *)0x77100170))


#endif /*__S5PV210_H__*/
