.text
.global mem_init
#define ELFIN_MEM_SYS_CFG	0x7e00f120
#define ELFIN_DMC1_BASE 0x7e001000
mem_init:
		ldr r0, =ELFIN_MEM_SYS_CFG
		mov r1,#0xd
		str r1, [r0]
		
		ldr r0, =ELFIN_DMC1_BASE
		mov r1, #0x04
		str r1,[r0,#0x04]
		
		ldr r0, =0x7e001014  @CAS latency¼Ä´æÆ÷
    mov r1, #(3 << 1)
    str r1, [r0]

    ldr r0, =0x7e001018  @t_DQSS¼Ä´æÆ÷
    mov r1, #0x1
    str r1, [r0]

    ldr r0, =0x7e00101c  @T_MRD¼Ä´æÆ÷
    mov r1, #0x2
    str r1, [r0]

    ldr r0, =0x7e001020   @t_RAS¼Ä´æÆ÷
    ldr r1, =( ( 45 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]

    ldr r0, =0x7e001024   @t_RC¼Ä´æÆ÷
    ldr r1, =( ( 68 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]

    ldr r0, =0x7e001028   @t_RCD¼Ä´æÆ÷
    ldr r1, =( ( 23 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]

    ldr r0, =0x7e00102c   @t_RFC¼Ä´æÆ÷
    ldr r1, =( ( 80 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]

    ldr r0, =0x7e001030   @t_RP¼Ä´æÆ÷
    ldr r1, =( ( 23 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]

    ldr r0, =0x7e001034   @t_rrd¼Ä´æÆ÷
    ldr r1, =( ( 15 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]

    ldr r0, =0x7e001038   @t_wr¼Ä´æÆ÷
    ldr r1, =( ( 15 / ( 1000000000 / 133000000 ) + 1 ) )
 @  ldr r2, [r0]
    str r1, [r0]

    ldr r0, =0x7e00103c   @t_wtr¼Ä´æÆ÷
    mov r1, #0x07
    str r1, [r0]

    ldr r0, =0x7e001040   @t_xp¼Ä´æÆ÷
    mov r1, #0x02
    str r1, [r0]

    ldr r0, =0x7e001044   @t_xsr¼Ä´æÆ÷
    ldr r1, =( ( 120 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]

    ldr r0, =0x7e001048   @t_esr¼Ä´æÆ÷
    ldr r1, =( ( 120 / ( 1000000000 / 133000000 ) + 1 ) )
    str r1, [r0]

    ldr r0, =0x7e00100c   @ÄÚ´æ¿ØÖÆÅäÖÃ¼Ä´æÆ÷
    ldr r1, =0x00010012   @ÅäÖÃ¿ØÖÆÆ÷
    str r1, [r0]

    ldr r0, =0x7e00104c   @32Î»DRAMÅäÖÃ¿ØÖÆ¼Ä´æÆ÷
    ldr r1, =0x0b45
    str r1, [r0]

    ldr r0, =0x7e001200   @Æ¬Ñ¡¼Ä´æÆ÷
    ldr r1, =0x150f8
    str r1, [r0]

    ldr r0, =0x7e001304   @ÓÃ»§ÅäÖÃ¼Ä´æÆ÷
    mov r1, #0x0
    str r1, [r0]
		
		ldr r0, =0x7E001008
		ldr r1, =0x000c0000
		str r1, [r0]
		
		ldr r1, =0x00000000
		str r1, [r0]
		
		ldr r1, =0x00040000
		str r1, [r0]
		str r1, [r0]
		
		ldr r1, =0x000a0000
		str r1, [r0]

		ldr r1, =0x00080032
		str r1, [r0]
		
		ldr r0, =ELFIN_DMC1_BASE
		mov r1, #0x00
		str r1,[r0,#0x04]

check_dmc1_ready:
		ldr r0, =0x7E001000
		ldr r1, [r0]
		mov r2, #0x03
		and r1, r1 , r2
		cmp r1, #0x01
		bne check_dmc1_ready
		nop
		mov pc, lr
		
		