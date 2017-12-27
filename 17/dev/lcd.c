//#include "lcd.h"
#include "int.h"
#include "s3c6410.h"




void timer0_irq_init(void)
{	
	/* 在中断控制器里使能timer0中断 */	
	VIC0INTENABLE |= (1<<23);
}
// timer0中断的中断处理函数
int do_irqi=0;
void do_irq()
{	
	__asm__(
	//保护环境，因为流水线，pc+12，lr+8
	"sub lr, lr, #4\n"
	"stmfd sp!, {r0-r12,lr}\n"
	:
	:
	);
	VIC0INTENCLEAR|=(1<<23)|(1<<27);
	do_irqi++;
	printf1("do_irqi:%d\n",do_irqi);
	unsigned long uTmp;	
	//清timer0的中断状态寄存器	
	uTmp = TINT_CSTAT;	
	TINT_CSTAT = uTmp; 
	VIC0ADDRESS = 0;
	VIC1ADDRESS = 0;
	VIC0INTENABLE|=(1<<27)|(1<<23);
	__asm__(
	//^意思是把SPSR拷贝到CPSR
	"sub lr, lr, #4\n"
	"ldmfd sp!, {r0-r12,pc}^\n"
	:
	:
	);
}

void timer_init(void)
{	
	unsigned long temp0;
	unsigned long utimer = 0;
	unsigned long uprescaler = 65;
	unsigned long udivider = 4;
	unsigned long utcntb = 62500;
	unsigned long utcmpb = 0;
	printf("%s\r\n",__FUNCTION__);
	timer0_irq_init();
	// 定时器的输入时钟 = PCLK / ( {prescaler value + 1} ) / {divider value} = PCLK/(65+1)/16=62500hz	
	//设置预分频系数为66	
	temp0 = TCFG0;	
	temp0 = (temp0 & (~(0xff00ff))) | ((66-1)<<0);	
	TCFG0 = temp0;
	// 16分频
	temp0 = TCFG1;
	temp0 = (temp0 & (~(0xf<<4*utimer))& (~(1<<20))) |(udivider<<4*utimer);
	TCFG1 = temp0;
	// 1s = 66Mhz	
	TCNTB0 = utcntb;
	TCMPB0 = utcmpb;

	// 手动更新	
	TCON |= 1<<1;	

	// 清手动更新位	
	TCON &= ~(1<<1);	

	// 自动加载和启动timer0	
	TCON |= (1<<0)|(1<<3);

	pISR_TIMER0 = (unsigned long)do_irq;
	intc_enable(NUM_TIMER0);
	
	// 使能timer0中断	
	temp0 = TINT_CSTAT;	
	temp0 = (temp0 & (~(1<<0)))|(1<<(0));	
	TINT_CSTAT = temp0;
}







enum {
	IDLE,
	START,
	REQUEST,
	WAITING,
	RESPONSE,
	STOPING,
} one_wire_status = IDLE;

static volatile unsigned int io_bit_count;
static volatile unsigned int io_data;
static volatile unsigned char one_wire_request;

#define SAMPLE_BPS 9600
#define SLOW_LOOP_FEQ 25
#define FAST_LOOP_FEQ 60

// 三类要求:触摸、型号、背光
#define REQ_TS   0x40U		// 触摸要求
#define REQ_INFO 0x60U		// 型号要求
int gLCD_Type;
#define RGB(r,g,b)   (unsigned int)( (r << 16) + (g << 8) + b )

static int ts_ready;
static unsigned ts_status;
static int resumed = 0;


static const unsigned char crc8_tab[] = {
0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,
0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,
0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,
0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,
0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,
0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,
0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,
0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,
0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,
0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3,
};
static int bl_ready;
static unsigned char backlight_req = 0;
static unsigned char backlight_init_success;

static  void notify_bl_data(unsigned char a, unsigned char b, unsigned char c)
{
	bl_ready = 1;
	backlight_init_success = 1;
}

static void bl_write(int brightness)
{


	if (brightness> 127) {
		brightness= 127;
	}

	bl_ready = 0;
	backlight_req = brightness + 0x80U;
}


/////////////////////////////////////////////型号///////////////////////////////////////////////
static unsigned lcd_type, firmware_ver;
static  void notify_info_data(unsigned char _lcd_type,
		unsigned char ver_year, unsigned char week)
{
	if (_lcd_type != 0xFF) {
		lcd_type = _lcd_type;
		gLCD_Type = lcd_type;
		//printf("lcd type:%x\r\n",lcd_type);
		firmware_ver = ver_year * 100 + week;
	}
}

static  void notify_ts_data(unsigned x, unsigned y, unsigned down)
{
	//Uart_Printf("x = %d,y = %d down = %d\n",x,y,down);

	if(down)
	{
		down = 0;
	}
	if (!down && !(ts_status &(1U << 31))) {
		// up repeat, give it up
		return;
	}

	
	ts_status = ((x << 16) | (y)) | (down << 31);
	ts_ready = 1;
}
#define crc8_init(crc) ((crc) = 0XACU)
#define crc8(crc, v) ( (crc) = crc8_tab[(crc) ^(v)])

// once a session complete
static unsigned total_received, total_error;
static unsigned last_req, last_res;
static int cnt = 0;

static void one_wire_session_complete(unsigned char req, unsigned int res)
{
	unsigned char crc;
	const unsigned char *p = (const unsigned char*)&res;
	total_received ++;


	last_res = res;


	crc8_init(crc);
	crc8(crc, p[3]);
	crc8(crc, p[2]);
	crc8(crc, p[1]);

	if (crc != p[0]) {
		// 调试时可以打开，不会影响时序
		printf1("CRC dismatch\r\n");
		if (total_received > 100) {
			total_error++;			
		}
		return;
	}

	printf1("CRC match\r\n");
	switch(req) {
		// 触摸要求
		case REQ_TS:
			{
				unsigned short x,y;
				unsigned pressed;
				x =  ((p[3] >>   4U) << 8U) + p[2];
				y =  ((p[3] &  0xFU) << 8U) + p[1];
				pressed = (x != 0xFFFU) && (y != 0xFFFU);

				// 上报触摸数据
				//Uart_Printf("notify_ts_data\n");
				notify_ts_data(x, y, pressed);
			break;
			}
		// 型号要求
		case REQ_INFO:
			{
				// 上报信息数据
				//Uart_Printf("notify_info_data\n");
				notify_info_data(p[3], p[2], p[1]);
				break;
			}
		// 背光要求	
		default:
			{
				// 上报背光数据
				//Uart_Printf("notify_bl_data\n");
				notify_bl_data(p[3], p[2], p[1]);
				break;
			}
	}
}
static void set_pin_as_input(void)
{
	unsigned tmp;
	tmp = GPFCON;
	tmp &= ~(3U << 30);
	GPFCON = tmp;
}

static void set_pin_as_output(void)
{
	unsigned tmp;
	tmp = GPFCON;
	tmp = (tmp & ~(3U << 30)) | (1U << 30);
	GPFCON = tmp;
}

static void set_pin_value(int v)
{
	unsigned tmp;
	tmp = GPFDAT;
	if (v) {
		tmp |= (1 << 15);
	} else {
		tmp &= ~(1 << 15);
	}
	GPFDAT = tmp;
}

static int get_pin_value(void)
{
	int v;
	unsigned long tmp;
	tmp = GPFDAT;
	v = !!(tmp & (1 << 15));
	return v;
}

static int set_pin_up(void)
{	
	unsigned tmp;
	tmp = GPFPUD;
	tmp = (tmp & ~(0x3 << 2*15)) | (0x2 << 2*15);
	GPFPUD = tmp;
}
static unsigned long TCNT_FOR_SAMPLE_BIT;
static unsigned long TCNT_FOR_FAST_LOOP;
static unsigned long TCNT_FOR_SLOW_LOOP;

// 初始化硬件定时器timer3
static void init_timer_for_1wire(void)
{
	unsigned int prescale1_value;

	// get prescaler
	prescale1_value = 1;

	// calc the TCNT_FOR_SAMPLE_BIT, that is one of the goal
	TCNT_FOR_SAMPLE_BIT = (unsigned long)(66700000 / (1 + 1) / SAMPLE_BPS - 1);
	TCNT_FOR_FAST_LOOP  = 66700000 / (1 + 1) / FAST_LOOP_FEQ - 1;
	TCNT_FOR_SLOW_LOOP  = 66700000 / (1 + 1) / SLOW_LOOP_FEQ - 1;
	// select timer 3, the 2rd goal
	TCFG1 &= ~(15<<12); 

}
static  void stop_timer_for_1wire(void)
{
	TCON &= ~(1<<16);
}
void intc_clearvectaddr(void)
{
	// VICxADDR:当前正在处理的中断的中断处理函数的地址
	VIC0ADDRESS = 0;
	VIC1ADDRESS = 0;
}
void intc_enable(unsigned long intnum)
{
	unsigned long temp;
	if(intnum<32)
	{
		temp = VIC0INTENABLE;
		temp |= (1<<intnum);
		VIC0INTENABLE = temp;
	}
	else if(intnum<64)
	{
		temp = VIC1INTENABLE;
		temp |= (1<<(intnum-32));
		VIC1INTENABLE = temp;
	}
	// NUM_ALL : enable all interrupt
	else 
	{
		VIC0INTENABLE = 0xFFFFFFFF;
		VIC1INTENABLE = 0xFFFFFFFF;
	}

}
#define GPKCON (volatile unsigned long*)0x7f008800
#define GPKDAT (volatile unsigned long*)0x7f008808
void timer_for_1wire_interrupt(void)
{
	
	__asm__(
	"sub lr, lr, #4\n"
	"stmfd sp!, {r0-r12,lr}\n"
	:
	:
	);
	VIC0INTENCLEAR|=(1<<27)|(1<<23);

	// 清中断
 	/*io_bit_count--;

 	//printf1("one_wire_status:%d\r\n",one_wire_status);
	switch(one_wire_status) 
	{
	case START:
		if (io_bit_count == 0) 
		{
			io_bit_count = 16;
			one_wire_status = REQUEST;
		}
		break;

	case REQUEST:
		// 16次，每次发1bit,把要求发出去(606a)
		set_pin_value(io_data & (1U << 31));
		io_data <<= 1;
		if (io_bit_count == 0) {
			io_bit_count = 2;
			one_wire_status = WAITING;
		}
		break;
		
	case WAITING:
		// 2次
		if (io_bit_count == 0) {
			io_bit_count = 32;
			one_wire_status = RESPONSE;
		}
		if (io_bit_count == 1) {
			set_pin_as_input();
			set_pin_value(1);
		}
		break;
		
	case RESPONSE:
		// 32次，
		io_data = (io_data << 1) | get_pin_value();
		if (io_bit_count == 0) {
			io_bit_count = 2;
			one_wire_status = STOPING;
			set_pin_value(1);
			set_pin_as_output();
			// 通信完成，将要求和通信结果都传过去
			one_wire_session_complete(one_wire_request, io_data);
		}
		break;

	case STOPING:
		if (io_bit_count == 0) {
			one_wire_status = IDLE;
			stop_timer_for_1wire();
		}
		break;
		
	default:
		stop_timer_for_1wire();
	}*/
	unsigned long uTmp;	
	//清timer0的中断状态寄存器	
	TINT_CSTAT |= 0x108;
	VIC0ADDRESS = 0;
	VIC1ADDRESS = 0;
	VIC0INTENABLE|=(1<<27)|(1<<23);
	__asm__(
	"ldmfd sp!, {r0-r12,pc}^\n"
	:
	:
	);

}
static void start_one_wire_session(unsigned char req)
{
	unsigned char crc;

	if (one_wire_status != IDLE) {
		return;
	}

	one_wire_status = START;

	set_pin_value(1);
	set_pin_as_output();

	crc8_init(crc);
	crc8(crc, req);
	io_data = (req << 8) + crc;
	io_data <<= 16;
	// Uart_Printf("start_one_wire_session crc = %x req = %x\n", crc, req);	// crc = 0x6a	req = 0x60 	io_data= 0x606a 0000

	last_req = (io_data >> 16);												// last_req = 0x606a
	one_wire_request = req;							
	io_bit_count = 1;
	set_pin_as_output();

	// 设置计数值
	TCNTB3 = TCNT_FOR_SAMPLE_BIT;
	TCMPB3 = 0;

	// 启动timer3
	TCON &= ~(0xF << 16);
	TCON |= (1<<16)|(1<<17)|(1<<19);
	TCON &= ~(1<<17);

	set_pin_value(0);
}


void One_Wire_Timer_Proc()
{
	unsigned char req;

	// 还未识别LCD类型时，第一个要求是型号要求
	if (lcd_type == 0) 
	{
		req = REQ_INFO;
	} 
	// 背光要求
	else if (!backlight_init_success) 
	{
		req = 127;
	} 
	else if (backlight_req) 
	{
		req = backlight_req;
		backlight_req = 0;
	}
	// 触摸要求
	else 
	{
		req = REQ_TS;
	}
	printf("req:%d\r\n",req);

	start_one_wire_session(req);

}
void TS_1wire_Init()
{
	set_pin_as_output();
	set_pin_value(1);
	set_pin_up();

	// 初始化硬件定时器timer3
	init_timer_for_1wire();

	// 使能timer3的中断
	pISR_TIMER3 = (unsigned long)timer_for_1wire_interrupt;
	intc_enable(NUM_TIMER3);
	TINT_CSTAT |= 0x108;

	// 开始一线处理
	One_Wire_Timer_Proc();
}
/*
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
//pwm波形
#define TCFG0 (*((volatile unsigned long *)0x7F006000)) 
#define TCFG1 (*((volatile unsigned long *)0x7F006004)) 
#define TCON (*((volatile unsigned long *)0x7F006008))
#define TCNTB0 (*((volatile unsigned long *)0x7F00600C)) 
#define TCMPB0 (*((volatile unsigned long *)0x7F006010)) 
#define TCNTB1 (*((volatile unsigned long *)0x7F006018)) 
#define TCMPB1 (*((volatile unsigned long *)0x7F00601c)) 


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
#define DITHMODE        (*((volatile unsigned long *)0x77100170))*/

#define FRAME_BUFFER   0x54000000
extern unsigned char bmp[189000];
#define ROW				272
#define COL				480
#define HSPW 				(2)
#define HBPD			 	(40- 1)
#define HFPD 				(5 - 1)
#define VSPW				(2)
#define VBPD 				(8 - 1)
#define VFPD 				(9 - 1)
#define LINEVAL 			(271)
#define HOZVAL				(479)

void lcd_back_light(void)
{
        GPECON = 0x00011111;
        GPEDAT = 0x00000001;
        GPFCON |=(2<<30);
#if 1   
        TCFG0 = 0;
        TCFG1 |= (1<<4)|(1<<0);
        TCNTB0=3;
        TCMPB0=1;
        TCNTB1=3;
        TCMPB1=1;
        TCON |= 1<<1;
        TCON &= ~(1<<1);
        TCON |= 1<<9;
        TCON &= ~(1<<9);
        TCON |=(1<<8)|(1<<11)|(1<<0)|(1<<3);
#endif


}
// 初始化LCD
void lcd_init(void)
{
	printf("mhw\r\n");
	int i=0;
	TS_1wire_Init();
	while(1)
		printf("i:%d\n\r",i++);
	while(!gLCD_Type)
	{
		One_Wire_Timer_Proc();
		printf("gLCD_Type:%d\n",gLCD_Type);
	}
	while(!backlight_init_success)
	{
		One_Wire_Timer_Proc();

	}
	TCON &= ~(1<<0);
	//lcd_back_light();
	// 配置GPIO用于LCD相关的功能
	GPECON = 0x00011111;
	GPEDAT = 0x00000001;	
	GPFCON = 0x96AAAAAA;
	GPFDAT = 0x00002000;
	GPICON = 0xAAAAAAAA;
	GPJCON = 0x00AAAAAA;

	// normal mode
	MIFPCON &= ~(1<<3);

	// RGB I/F
	SPCON =  (SPCON & ~(0x3)) | 1;

	// 配置VIDCONx，设置接口类型、时钟、极性和使能LCD控制器等
	VIDCON0 = (0<<26)|(0<<17)|(0<<16)|(12<<6)|(0<<5)|(1<<4)|(0<<2)|(3<<0);
	VIDCON1 |= 1<<5 | 1<<6;

	// 配置VIDTCONx，设置时序和长宽等
	// 设置时序
	VIDTCON0 = VBPD<<16 | VFPD<<8 | VSPW<<0;
	VIDTCON1 = HBPD<<16 | HFPD<<8 | HSPW<<0;
	// 设置长宽
	VIDTCON2 = (LINEVAL << 11) | (HOZVAL << 0);


	// 配置WINCON0，设置window0的数据格式
	WINCON0 |= 1<<0;
	WINCON0 &= ~(0xf << 2);
	WINCON0 |= 0xB<<2;

	// 配置VIDOSD0A/B/C,设置window0的坐标系
#define LeftTopX     0
#define LeftTopY     0
#define RightBotX   479
#define RightBotY   271
	VIDOSD0A = (LeftTopX<<11) | (LeftTopY << 0);
	VIDOSD0B = (RightBotX<<11) | (RightBotY << 0);
	VIDOSD0C = (LINEVAL + 1) * (HOZVAL + 1);

	// 置VIDW00ADD0B0和VIDW00ADD1B0，设置framebuffer的地址
	VIDW00ADD0B0 = FRAME_BUFFER;
	VIDW00ADD1B0 = (((HOZVAL + 1)*4 + 0) * (LINEVAL + 1)) & (0xffffff);
	return;
}

// 描点
void lcd_draw_pixel(int row, int col, int color)
{
	unsigned long * pixel = (unsigned long  *)FRAME_BUFFER;

	*(pixel + row * COL + col) = color;

}

// 清屏
void lcd_clear_screen(int color)
{
	int i, j;

	for (i = 0; i < ROW; i++)
		for (j = 0; j < COL; j++)
			lcd_draw_pixel(i, j, color);

}

// 划横线
void lcd_draw_hline(int row, int col1, int col2, int color)
{
	int j;

	// 描第row行，第j列
	for (j = col1; j <= col2; j++)
		lcd_draw_pixel(row, j, color);

}

// 划竖线
void lcd_draw_vline(int col, int row1, int row2, int color)
{
	int i;
	// 描第i行，第col列
	for (i = row1; i <= row2; i++)
		lcd_draw_pixel(i, col, color);

}

// 划十字
void lcd_draw_cross(int row, int col, int halflen, int color)
{
	lcd_draw_hline(row, col-halflen, col+halflen, color);
	lcd_draw_vline(col, row-halflen, row+halflen, color);
}

// 绘制同心圆
void lcd_draw_circle(void)
{
	int x,y;
	int color;
	unsigned char red,green,blue,alpha;
	int xsize = ROW;
	int ysize = COL;

	for (y = 0; y < ysize; y++)
		for (x = 0; x < xsize; x++)
		{
			color = ((x-xsize/2)*(x-xsize/2) + (y-ysize/2)*(y-ysize/2))/64;
			red   = (color/8) % 256;
			green = (color/4) % 256;
			blue  = (color/2) % 256;
			alpha = (color*2) % 256;

			color |= ((int)alpha << 24);
			color |= ((int)red   << 16);
			color |= ((int)green << 8 );
			color |= ((int)blue       );

			lcd_draw_pixel(x,y,color);
		}
}
void point(int row, int col, int color)
{
    unsigned long *point = (unsigned long*)FRAME_BUFFER;
    *(point + row*480 + col) = color;
}

void Paint_Bmp(const unsigned char gImage_bmp[])
{
	int i, j;
	unsigned char *p = (unsigned char *)gImage_bmp;
	int blue, green, red;
	int color;

	// 图片大小200x200像素
	for (i = 0; i < 200; i++)
		for (j = 0; j < 200; j++)
		{
			blue  = *p++;
			green = *p++;
			red   = *p++;
			
			// D[23:16] = Red data, D[15:8] = Green data,D[7:0] = Blue data
			
			color = (red << 18)| (green << 10)|( blue << 2); 
			point(i, j, color);
		}
}

void lcd_test()
{
	int y;
	
	//划横线 -> 描第150行，第y列
	for(y=100;y<380;y++)

		point(210,y,0xFFFF00);	
	
	Paint_Bmp(bmp);
	//lcd_clear_screen(0x0000ff);
	
}



