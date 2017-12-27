@****************************
@File:start.S
@
@���ΰ
@****************************

.text
.global _start
_start:
		b	reset						
		ldr	pc, _undefined_instruction	
		ldr	pc, _software_interrupt		
		ldr	pc, _prefetch_abort			
		ldr	pc, _data_abort				
		ldr	pc, _not_used				
		ldr	pc, _irq					
		ldr	pc, _fiq

_undefined_instruction: .word undefined_instruction
_software_interrupt:	.word software_interrupt
_prefetch_abort:	.word prefetch_abort
_data_abort:		.word data_abort
_not_used:		.word not_used
_irq:			.word irq
_fiq:			.word fiq					

undefined_instruction:
		nop

software_interrupt:
		nop

prefetch_abort:
		nop

data_abort:
		nop

not_used:
		nop

irq:
		nop

fiq:
		nop

reset:
		bl set_svc
		bl set_peri_port
		bl disable_watchdog
		bl disable_interrupt
		bl disable_mmu
		bl clock_init
		bl mem_init
		bl init_stack
		bl nand_init
@		bl copy_to_ram
		@bl copy_nand_to_ram
		bl led_on
		bl clean_bss
		ldr pc, =gboot_main
		
//����������ַ����������ַд��cp15 c15 c2�Ĵ��� 
set_peri_port:
		ldr r0, =0x70000000
		orr r0, r0, #0x13
		mcr p15, 0, r0, c15, c2, 4
		mov pc, lr

//����cpu��SVCģʽ����0xd3д��cpsr��		
set_svc:
		mrs r0, cpsr
		bic r0, r0, #0x1f
		orr r0, r0, #0xd3
		msr cpsr, r0
		mov pc, lr

//����watchdog����WATCON�����λ����������watchdog 
#define pWTCON 0x7e004000
disable_watchdog:
		ldr r0, =pWTCON
		mov r1, #0x0
		str r1, [r0]
		mov pc, lr
		
//�����жϣ������VICxINTENCLEAR�Ĵ���д��ȫ1������ж�ʹ�ܡ� 
disable_interrupt:
		ldr r0,	=0x71200014
		mvn r1, #0x0
		str r1, [r0]
		ldr r0,	=0x71300014
		str r1, [r0]
		mov pc, lr

//����mmu��	1.��cp15 c7 c7 д���ⶫ����ʹ I/D cache��Ч 
//			2.��cp15 c1 c0 ��0,2λд��0���ֱ����mmu��d cache 
disable_mmu:
		//����I/D cache 
		mcr p15, 0, r0, c7, c7, 0
		mrc p15, 0, r0, c1, c0, 0
		bic r0, r0, #0x05
		mcr p15, 0, r0, c1, c0, 0
		mov pc, lr

/*
arm:	533M
HCLKx2:	266M
HCLK:	133M
PCLK:	66M
*/ 
#define OTHERS 0x7E00F900
#define APLL_CON 0x7E00F00C
#define MPLL_CON 0x7E00F010
#define PLL_VALUE ((1<<31)|(266<<16)|(3<<8)|(1<<0)) 
#define CLK_SRC 0x7E00F01C 
#define CLK_DIV0 0x7E00F020
#define DIV_VAL ((0<<0)|(0<<4)|(1<<9)|(1<<8)|(3<<12))
clock_init:
		//��Ϊcpu���������ߵ�ʱ�Ӳ�ͬ���������cpu���첽ģʽ 
		ldr r0, =OTHERS
		ldr r1, [r0]
		bic r1,r1, #0xc0
		str r1, [r0]
		
		//armclock=APLL��PLLclock=MPLL/2,HCLK=HCLKx2 /2,HCLKx2=HCLKx2in /2,PCLK=HCLKx2 /4
		ldr r0, =CLK_DIV0
		ldr r1, =DIV_VAL
		str r1, [r0]
		
		//����APLL��533M 
		ldr r0, =APLL_CON
		ldr r1,=PLL_VALUE
		str r1, [r0]

		//����MPLL��533M
		ldr r0, =MPLL_CON
		ldr r1, =PLL_VALUE
		str r1, [r0]
		
		//Ϊ����clockѡ����ȷ��ʱ��Դ 
		ldr r0, =CLK_SRC
		mov r1, #0x03
		str r1, [r0]
		mov pc,lr
//�������룺�ӵ��ʯ����4K���ڴ��0x50008000�� 
copy_to_ram:
		ldr r0, =0x0C000000
		ldr r1, =0x50008000
		add r3, r0, #1024*4

copy_loop:
		ldr r2 , [r0] , #4
		str r2 , [r1] , #4
		cmp r0, r3
		bne copy_loop
		
		mov pc, lr

copy_nand_to_ram:
		mov r0, #0
		ldr r1,	=_start
		ldr r2, =bss_end
		sub r2, r2, r1
		mov ip, lr
		bl nand_to_ram
		mov lr, ip
		mov pc, lr

//���ջ������cpu�����ж�ģʽ�������жϵ�ջ��0x5300000-0x53ffffff
//		  ��cpu����svcģʽ������svc��ջ��0x54000000-0x5fffffff		
init_stack:
		msr cpsr, #0xd2
		ldr sp, =0x54000000
		msr cpsr, #0xd3
		ldr sp, =0x53000000
		mov pc, lr

//���bss�Σ�bss�ο�ͷ��β����lds�ļ��ж����˱�š� 
clean_bss:
		ldr r0, =bss_start
		ldr r1, =bss_end
		cmp r0, r1
		moveq pc, lr
clean_loop:
		mov r2, #0
		str r2, [r0], #4
		cmp r0, r1
		moveq pc, lr
		b clean_loop
		
#define GPKCON0 0x7F008800	
#define GPKDAT 0x7F008808	
led_on:
		ldr r0, =GPKCON0
		ldr r1, =0x11110000
		str r1, [r0]
		ldr r0, =GPKDAT
		ldr r1, =0x00a0
		str r1, [r0]
		mov pc, lr
		

