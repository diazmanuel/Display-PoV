/*
 * Timers.c
 *
 *  Created on: May 17, 2018
 *      Author: germi
 */

#include "Timers.h"
#include "InterruptHandler.h"
#include "RegsLPC1769.h"
#include "GPIOLPC1769.h"
#include "Shifter.h"
#include "Display.h"
#include "HallSensor.h"




/*
 * Timer 0: Shifter/Latch Clock + PWM
 * Calculo: LatchFreqMIN*3Shifters
 * 			50 Hz 	*	(8+8+8)		=1200 Hz
 * Periodo maximo de shifter = 1/1200Hz = 833 us.
 *
 */
void InitTimer0 (void){
		PCONP |= (0x01 << 1); 	//Power Control for Peripherals registers.

		PCLKSEL0 &=~ (0x03<<2);
		PCLKSEL0 |= (0x01<<2);		//PCLK = CLK

		T0TCR &=~(1<<CE);		//paramo to2
		T0TCR |= (1<<CR);

		T0CTCR &=~(0x03);		//Funcion : timer.
		T0MCR &=~(0xFF);		//limpiamos por si las weas
		T0MCR |= (1<<MR0I);		//reset on mr0
		T0MCR |= (1<<MR0R);
		T0MCR |= (1<<MR1I);


		T0MR0=	PERIODO_LATCH/(8*CANT_SHIFTERS);		//PERIODO
		T0MR1=	T0MR0/2;		//Duty Cycle 50%

		ISER0|=0x02; 		//habilitamos IR
}

/*
 * Timer 1: Resfresco de datos a mostrar
 * 	Principio de funcionamiento:
 * 	Este timer sera inicializado con periodo de trabajo tal que sea facil llevar la cuenta de
 * 	tiempos para que, al iniciar el programa, tendra un tiempo de demora hasta que el motor arranque,
 * 	llegue a una velocidad de trabajo estable, la cual será sensada y calculada por HallSensor_t,
 * 	la velocidad estable será traducida y almacenada en HallSensor_t->WorkingFrequency que a su vez,
 * 	dicha variable SOBRE ESCRIBIRA los registros T1MR0 y T1MR1 para adecuar los periodos de trabajo
 * 	a los que se debe realmente refreshear el dato (2,8º)
 */
void InitTimer1(void){	//Usar counter mode con pin de CAP donde va la señal de HALL por flanco ascendente


	PCONP |= (0x01 << 2);

	PCLKSEL0 &=~ (0x03<<4);
	PCLKSEL0 |= (0x01<<4);		//PCLK = CLK

	T1TCR &=~(1<<CE);		//paramo to2
	T1TCR |= (1<<CR);

	T1CTCR &=~(0x03);		//Funcion : timer.
	T1MCR &=~(0xFF);		//limpiamos por si las weas
	/*T1MCR |= (1<<MR0I);		//IR, reset on mr0
	T1MCR |= (1<<MR0R);
	T1MCR |= (1<<MR1I);

	T1MR0=	US_1;		//PERIODO
	T1MR1=	(US_1)/2;

	ISER0|=(0x01<<2); 		//habilitamos IR
	*/
	T1TCR &=~(1<<CR);
	T1TCR |=(1<<CE);

}

/*
 * Timer 0 = Timer encargado de la logica de shifteo y latcheo.
 * Que tambien se encarga de cambiar PwmStateIndex (A la frecuencia de latcheo)
 */

void TIMER0_IRQHandler(void)
{
	if( T0IR & (1 << IRMR0))// Si interrumpio Match 0
	{
		 T0IR |=(1 << IRMR0 );	// Borro flag del Match 0
		 SetPIN(SCLK_PIN,1);	// El SCLK es activo por flanco ascendente.
		 ShiftCounter++;
		 if(ShiftCounter==(8*CANT_SHIFTERS))
		 {
			 SetPIN(LCLK_PIN,0);
			 ShiftCounter=0;
			 PwmStateIndex++;
			 PwmStateIndex%=3;
		 }
		 Interrupt_Flags|=(ENABLE<<SHIFTDATA_READY);
		 /*
		  * IMPORTANTE! LA ATENCION DE ESTA INTERRUPCION DEBE SER MAS RAPIDA QUE UN CICLO
		  * DEL SCLK PARA QUE CUANDO COMPLETE SU PERIODO, HAYA UN DATO DISPONIBLE PARA
		  * SHIFTEAR
		  */

	}
	if( T0IR & (1 << IRMR1))	// Si interrumpio Match 1
	{
		T0IR |=(1 << IRMR1 ); 	// Borro flag del Match 1
		SetPIN(SCLK_PIN,0);
		 if(ShiftCounter==0)
		 {
			 SetPIN(LCLK_PIN,1);	//Hasta aca, se ve la primer linea printeada.
			 /*
			  * Bug Documentado: El primer print se va a hacer con basura de datos previos.
			  */
		 }
	}
}

/*
 * Esta tiene que ser la atencion de interrupcion mas rapida del lejano oeste
 */
void TIMER1_IRQHandler(void)
{
	if(T1IR & (1<<IRMR0))
	{
		T1IR|=(1<<IRMR0);
		Interrupt_Flags|=(ENABLE<<REFRESHDATA_READY);
	}
}

void RefreshTimerSetup(HallSensor_t *This_HallSensor)
{
	T1TCR &=~(1<<CE);		//paramo to2
	T1TCR |= (1<<CR);

	T1MCR |= (1<<MR0I);		//IR, reset on mr0
	T1MCR |= (1<<MR0R);
	T1MR0=	This_HallSensor->WorkingFrequency/FILAS_IMAGEN;		//Deberia refreshear el dato cada 2.8º aprox

	ISER0|=(0x01<<2); 		//habilitamos IR

	T1TCR &=~(1<<CR);
	T1TCR |=(1<<CE);

}
/*
 * Por si quiero re-sincronizar todo denuevo a la mitad del programa.
 * Agregar EXT1 con un pulsador externo.
 */
void RefreshTimerReset(void)
{

}
