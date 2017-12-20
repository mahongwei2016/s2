#include "vsprintf.h"
unsigned char outbuff[1024];
unsigned char inbuff[1024];
int printf(const char* fmt,...)
{
	va_list args;
	int i;
	//1.将变参转换为字符串
	va_start(args,fmt);
	vsprintf((char*)outbuff,fmt,args);
	va_end();
	//2.打印字符串到串口
	for(i=0;i<strlen(outbuff);i++)
		putc(outbuff[i]); 
	return 1;
}
int scanf(const char* fmt,...)
{
	unsigned char c;
	va_list args;
	int i=0;
	//获取输入的字符串
	while(1)
	{
		c=getc();
		if((c==0x0d)||(c==0x0a))
		{
			inbuff[i]='\n';
			break;	
		}
		else
		{
			inbuff[i++]=c;	
		}	
	} 
	//格式转换
	va_start(args,fmt); 
	vsscanf((char*)inbuff,fmt,args);
	va_end(args);
	return i;
}
