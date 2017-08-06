//#include "lcd.h"

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

#define FRAME_BUFFER   0x54000000
extern unsigned char bmp[189000];
#define ROW				272
#define COL				480
#define HSPW 				(2)
#define HBPD			 	(16- 1)
#define HFPD 				(16 - 1)
#define VSPW				(2)
#define VBPD 				(4 - 1)
#define VFPD 				(2 - 1)
#define LINEVAL 			(271)
#define HOZVAL				(479)


// ��ʼ��LCD
void lcd_init(void)
{
	// ����GPIO����LCD��صĹ���
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

	// ����VIDCONx�����ýӿ����͡�ʱ�ӡ����Ժ�ʹ��LCD��������
	VIDCON0 = (0<<26)|(0<<17)|(0<<16)|(16<<6)|(0<<5)|(1<<4)|(0<<2)|(3<<0);
	VIDCON1 |= 1<<5 | 1<<6;

	// ����VIDTCONx������ʱ��ͳ����
	// ����ʱ��
	VIDTCON0 = VBPD<<16 | VFPD<<8 | VSPW<<0;
	VIDTCON1 = HBPD<<16 | HFPD<<8 | HSPW<<0;
	// ���ó���
	VIDTCON2 = (LINEVAL << 11) | (HOZVAL << 0);


	// ����WINCON0������window0�����ݸ�ʽ
	WINCON0 |= 1<<0;
	WINCON0 &= ~(0xf << 2);
	WINCON0 |= 0xB<<2;

	// ����VIDOSD0A/B/C,����window0������ϵ
#define LeftTopX     0
#define LeftTopY     0
#define RightBotX   479
#define RightBotY   271
	VIDOSD0A = (LeftTopX<<11) | (LeftTopY << 0);
	VIDOSD0B = (RightBotX<<11) | (RightBotY << 0);
	VIDOSD0C = (LINEVAL + 1) * (HOZVAL + 1);

	// ��VIDW00ADD0B0��VIDW00ADD1B0������framebuffer�ĵ�ַ
	VIDW00ADD0B0 = FRAME_BUFFER;
	VIDW00ADD1B0 = (((HOZVAL + 1)*4 + 0) * (LINEVAL + 1)) & (0xffffff);
}

// ���
void lcd_draw_pixel(int row, int col, int color)
{
	unsigned long * pixel = (unsigned long  *)FRAME_BUFFER;

	*(pixel + row * COL + col) = color;

}

// ����
void lcd_clear_screen(int color)
{
	int i, j;

	for (i = 0; i < ROW; i++)
		for (j = 0; j < COL; j++)
			lcd_draw_pixel(i, j, color);

}

// ������
void lcd_draw_hline(int row, int col1, int col2, int color)
{
	int j;

	// ���row�У���j��
	for (j = col1; j <= col2; j++)
		lcd_draw_pixel(row, j, color);

}

// ������
void lcd_draw_vline(int col, int row1, int row2, int color)
{
	int i;
	// ���i�У���col��
	for (i = row1; i <= row2; i++)
		lcd_draw_pixel(i, col, color);

}

// ��ʮ��
void lcd_draw_cross(int row, int col, int halflen, int color)
{
	lcd_draw_hline(row, col-halflen, col+halflen, color);
	lcd_draw_vline(col, row-halflen, row+halflen, color);
}

// ����ͬ��Բ
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

	// ͼƬ��С200x200����
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
	
	//������ -> ���150�У���y��
	for(y=100;y<380;y++)

		point(210,y,0xFFFF00);	
	
	Paint_Bmp(bmp);
	//lcd_clear_screen(0x0000ff);
	
}



