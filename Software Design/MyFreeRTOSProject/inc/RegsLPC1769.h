/*
===============================================================================
RegsLPC1769
===============================================================================
*/
#ifndef REGS_H_
#define REGS_H_

#define		__R			volatile const
#define		__W			volatile
#define		__RW				volatile
typedef 	unsigned int 		uint32_t;
typedef 	unsigned short 		uint16_t;
typedef 	unsigned char 		uint8_t;
typedef 	__RW uint32_t 		registro_t;  //!< defino un tipo 'registro'.



// REGISTROS ------------------------------------------------------------------------------------------------------------------------
#define		MYPINSEL			( ( registro_t  * ) 0x4002C000UL )		//!< Direccion de inicio de los registros PINSEL
#define		MYPINMODE			( ( registro_t  * ) 0x4002C040UL )		//!< Direccion de inicio de los registros de modo de los pines del GPIO
#define		GPIO			( ( registro_t  * ) 0x2009C000UL )		//!< Direccion de inicio de los registros de GPIOs

#define		TIMER0			( ( registro_t  * ) 0x40004000UL )

#define		T0IR			TIMER0[ 0 ]			/** IR - INTERRUPT REGISTER */
	#define		IRMR0		0
	#define		IRMR1		1
	#define		IRMR2		2
	#define		IRMR3		3
	#define		IRCR0		4
	#define		IRCR1		5
#define		T0TCR			TIMER0[ 1 ]			/** TCR - TIMER CONTROL REGISTER */
	#define		CE			0
	#define		MYCR			1
#define		T0TC			TIMER0[ 2 ]			/** TC - TIMER COUNTER REGISTER */
#define		T0PR			TIMER0[ 3 ]			/** PR - PRESCALE REGISTER */
#define		T0PC			TIMER0[ 4 ]			/** PC - PRESCALE COUNTER REGISTER */
#define		T0MCR			TIMER0[ 5 ]			/** MCR - MATCH CONTROL REGISTER */
	#define		MR0I			0
	#define		MR0R			1
	#define		MR0S			2
	#define		MR1I			3
	#define		MR1R			4
	#define		MR1S			5
	#define		MR2I			6
	#define		MR2R			7
	#define		MR2S			8
	#define		MR3I			9
	#define		MR3R			10
	#define		MR3S			11
#define		T0MR0			TIMER0[ 6 ]			/** MR - MATCH CONTROL REGISTER */
#define		T0MR1			TIMER0[ 7 ]
#define		T0MR2			TIMER0[ 8 ]
#define		T0MR3			TIMER0[ 9 ]
#define		T0CCR			TIMER0[ 10 ]
#define		T0CR0			TIMER0[ 11 ]
#define		T0CR1			TIMER0[ 12 ]
//los siguientes dos registros NO estan contigüos. Por ende no se continúa con
//el offset
#define		T0EMR			( * ( ( registro_t  * ) 0x4000403CUL ) )
#define		T0CTCR			( * ( ( registro_t  * ) 0x40004070UL ) )/** CTCR - COUNT CONTROL REGISTER */
	#define		TCM			0
	#define		CIS			2

#define		TIMER1			( ( registro_t  * ) 0x40008000UL )

#define		T1IR			TIMER1[ 0 ]			/** IR - INTERRUPT REGISTER */
#define		T1TCR			TIMER1[ 1 ]			/** TCR - TIMER CONTROL REGISTER */
#define		T1TC			TIMER1[ 2 ]			/** TC - TIMER COUNTER REGISTER */
#define		T1PR			TIMER1[ 3 ]			/** PR - PRESCALE REGISTER */
#define		T1PC			TIMER1[ 4 ]			/** PC - PRESCALE COUNTER REGISTER */
#define		T1MCR			TIMER1[ 5 ]			/** MCR - MATCH CONTROL REGISTER */
#define		T1MR0			TIMER1[ 6 ]			/** MR - MATCH CONTROL REGISTER */
#define		T1MR1			TIMER1[ 7 ]
#define		T1MR2			TIMER1[ 8 ]
#define		T1MR3			TIMER1[ 9 ]
#define		T1CCR			TIMER1[ 10 ]
#define		T1CR0			TIMER1[ 11 ]
#define		T1CR1			TIMER1[ 12 ]
#define		T1CTCR			( * ( ( registro_t  * ) 0x40008070UL ) )

#define		TIMER2			( ( registro_t  * ) 0x40090000UL )

#define		T2IR			TIMER2[ 0 ]			/** IR - INTERRUPT REGISTER */
#define		T2TCR			TIMER2[ 1 ]			/** TCR - TIMER CONTROL REGISTER */
#define		T2TC			TIMER2[ 2 ]			/** TC - TIMER COUNTER REGISTER */
#define		T2PR			TIMER2[ 3 ]			/** PR - PRESCALE REGISTER */
#define		T2PC			TIMER2[ 4 ]			/** PC - PRESCALE COUNTER REGISTER */
#define		T2MCR			TIMER2[ 5 ]			/** MCR - MATCH CONTROL REGISTER */
#define		T2MR0			TIMER2[ 6 ]			/** MR - MATCH CONTROL REGISTER */
#define		T2MR1			TIMER2[ 7 ]
#define		T2MR2			TIMER2[ 8 ]
#define		T2MR3			TIMER2[ 9 ]
#define		T2CCR			TIMER2[ 10 ]
#define		T2CR0			TIMER2[ 11 ]
#define		T2CR1			TIMER2[ 12 ]
#define		T2CTCR			( * ( ( registro_t  * ) 0x40090070UL ) )

#define		TIMER3			( ( registro_t  * ) 0x40094000UL )

#define		T3IR			TIMER3[ 0 ]			/** IR - INTERRUPT REGISTER */
#define		T3TCR			TIMER3[ 1 ]			/** TCR - TIMER CONTROL REGISTER */
#define		T3TC			TIMER3[ 2 ]			/** TC - TIMER COUNTER REGISTER */
#define		T3PR			TIMER3[ 3 ]			/** PR - PRESCALE REGISTER */
#define		T3PC			TIMER3[ 4 ]			/** PC - PRESCALE COUNTER REGISTER */
#define		T3MCR			TIMER3[ 5 ]			/** MCR - MATCH CONTROL REGISTER */
#define		T3MR0			TIMER3[ 6 ]			/** MR - MATCH CONTROL REGISTER */
#define		T3MR1			TIMER3[ 7 ]
#define		T3MR2			TIMER3[ 8 ]
#define		T3MR3			TIMER3[ 9 ]
#define		T3CCR			TIMER3[ 10 ]
#define		T3CR0			TIMER3[ 11 ]
#define		T3CR1			TIMER3[ 12 ]
#define		T3CTCR			( * ( ( registro_t  * ) 0x40094070UL ) )


// NVIC ----------------------------------------------------------------------------------------------
#define		NVIC_TIMER0		1
// Nested Vectored Interrupt Controller (NVIC)
// 0xE000E100UL : Direccion de inicio de los registros de habilitación (set) de interrupciones en el NVIC:
#define		MYISER		( ( registro_t  * ) 0xE000E100UL )
//!< 0xE000E180UL : Direccion de inicio de los registros de deshabilitacion (clear) de interrupciones en el NVIC:
#define		MYICER		( ( registro_t  * ) 0xE000E180UL )

// Registros ISER:
#define		ISER0		MYISER[0]
#define		ISER1		MYISER[1]

// Registros ICER:
#define		ICER0		MYICER[0]
#define		ICER1		MYICER[1]

//  Power Control for Peripherals register (PCONP - 0x400F C0C4) [pag. 62 user manual LPC1769]
// 0x400FC0C4UL : Direccion de inicio del registro de habilitación de dispositivos:
#define 	MYPCONP	(* ( ( registro_t  * ) 0x400FC0C4UL ))

// Peripheral Clock Selection registers 0 and 1 (PCLKSEL0 -0x400F C1A8 and PCLKSEL1 - 0x400F C1AC) [pag. 56 user manual]
// 0x400FC1A8UL : Direccion de inicio de los registros de seleccion de los CLKs de los dispositivos:
#define		MYPCLKSEL		( ( registro_t  * ) 0x400FC1A8UL )
// Registros PCLKSEL
#define		PCLKSEL0	MYPCLKSEL[0]
#define		PCLKSEL1	MYPCLKSEL[1]

#define		PWM1		( ( registro_t  * ) 0x40018000UL )

#define 	PWM1IR		PWM1[0]
#define 	PWM1TCR		PWM1[1]
#define 	PWM1TC		PWM1[2]
#define 	PWM1PR		PWM1[3]
#define 	PWM1PC		PWM1[4]
#define 	PWM1MCR		PWM1[5]
#define 	PWM1MR0		PWM1[6]
#define 	PWM1MR1		PWM1[7]
#define 	PWM1MR2		PWM1[8]
#define 	PWM1MR3		PWM1[9]
#define 	PWM1PCR		PWM1[18]


//-------System Tick Timer-------------------


#define		SYSTICK		( ( registro_t  * ) 0xE000E010UL )
#define		STCTRL		SYSTICK[0]
	#define		ENB		0
	#define		MYINT			1
	#define		CLK			2
	#define		FLAG		16
#define		STRELOAD	SYSTICK[1]
#define		STCURR		SYSTICK[2]
#define		STCALIB		SYSTICK[3]

//  Interrupciones Externas  ------------------------------------------------------------------------------------------------
#define		NVIC_EINT0	18
#define		MYEXTINT 		( * ( ( registro_t  * ) 0x400FC140UL ) )
	#define		EINT0		0
	#define		EINT1		1
	#define		EINT2		2
	#define		EINT3		3

#define		MYEXTMODE 	( * ( ( registro_t  * ) 0x400FC148UL ) )
	#define		EXTMODE0	0
	#define		EXTMODE1	1
	#define		EXTMODE2	2
	#define		EXTMODE3	3
#define		MYEXTPOLAR 	( * ( ( registro_t  * ) 0x400FC14CUL ) )
	#define		EXTPOLAR0	0
	#define		EXTPOLAR1	1
	#define		EXTPOLAR2	2
	#define		EXTPOLAR3	3

//!< ///////////////////   UART0   //////////////////////////
//!< 0x4000C000UL : Registro de control de la UART0:
#define		UART0	( (__RW uint32_t *) 0x4000C000UL )
#define		UART2	( (__RW uint32_t *) 0x40098000UL )
//!< Registros de la UART0:
#define		U0THR		UART0[0]
#define		U0RBR		UART0[0]
#define		U0DLL		UART0[0]
#define		U2THR		UART2[0]
#define		U2RBR		UART2[0]
#define		U2DLL		UART2[0]

#define		U0DLM		UART0[1]
#define		U0IER		UART0[1]
#define		U2DLM		UART2[1]
#define		U2IER		UART2[1]

#define		U0IIR		((__R uint32_t *)UART0)[2]
#define		U0FCR		((__W uint32_t *)UART0)[2]
#define		U2IIR		((__R uint32_t *)UART2)[2]
#define		U2FCR		((__W uint32_t *)UART2)[2]

#define		U0LCR		UART0[3]
#define		U2LCR		UART2[3]
//!< posición 4 no definida [consultar pag. 300 user manual LPC1769]
#define		U0LSR		UART0[5]
#define		U2LSR		UART2[5]
//!< posición 6 no definida [consultar pag. 300 user manual LPC1769]
#define		U0SCR		UART0[7]
#define		U2SCR		UART2[7]
#define 	U0FDR		UART0[10]
#define 	U2FDR		UART2[10]

#endif
