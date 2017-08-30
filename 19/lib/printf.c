#include "vsprintf.h"
unsigned char outbuff[1024];
unsigned char inbuff[1024];
#define EINT0MASK (*((volatile unsigned long *)0x7F008920))
int printf(const char* fmt,...)
{
	va_list args;
	EINT0MASK|=(1<<7);
	int i;
	//1.�����ת��Ϊ�ַ���
	va_start(args,fmt);
	vsprintf((char*)outbuff,fmt,args);
	va_end();
	//2.��ӡ�ַ���������
	for(i=0;i<strlen(outbuff);i++)
		putc(outbuff[i]); 
	EINT0MASK&=(~(1<<7));
	return 1;
}
int scanf(const char* fmt,...)
{
	unsigned char c;
	va_list args;
	int i=0;
	//��ȡ������ַ���
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
	//��ʽת��
	va_start(args,fmt); 
	vsscanf((char*)inbuff,fmt,args);
	va_end(args);
	return i;
}
