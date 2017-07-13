@****************************
@File:start.S
@
@ÂíºêÎ°
@****************************

.text
.global	_start
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
		bl disable_watchdog
		bl disable_interrupt
		
set_svc:
		mrs r0, cpsr
		bic r0, r0, #0x1f
		orr r0, r0, #0xd3
		msr cpsr, r0
		mov pc, lr

#define pWTCON 0x7e004000
disable_watchdog:
		ldr r0, =pWTCON
		mov r1, #0x0
		str r1, [r0]
		mov pc, lr
		
#define VIC0SOFTINTCLEAR 0x7120001C
#define VIC1SOFTINTCLEAR 0x7130001C
disable_interrupt:
		ldr r0,	=VIC0SOFTINTCLEAR
		mvn r1, #0x0
		str r1, [r0]
		ldr r0,	=VIC1SOFTINTCLEAR
		str r1, [r0]
		mov pc, lr
