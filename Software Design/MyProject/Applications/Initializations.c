/*
===============================================================================
Initializations.c
===============================================================================
*/

#include "GPIOLPC1769.h"
#include "PinoutConfigLPC1769.h"
#include "Oscilador.h"
#include "RegsLPC1769.h"
#include "UART.h"
#include "Bluetooth.h"
#include "Initializations.h"
#include "Timers.h"
#include "Shifter.h"
#include "InterruptHandler.h"
#include "StatusHandler.h"


/********************************************************************************
	\fn  void Inicializar ( void )
	\brief Inicializacion de Hardware.
	\author & date: Informática II
 	\param void
	\return:	void
*/
void Inicializar ( void )
{
	InitPLL ( ) ;
	//Bluetooth_Init(115200);//tarda alrededor de 4 segundos en ejecutarse completamente
							 //debido a limitaciones del HC05 que hay que tener
							//en cuenta a la hora de RESETEARLO Y CONFIGURARLO
	InitInterruptFlags();
	InitStatusFlags();
	InitShifter();
	InitHallSensor();

	InitSystick();
	InitTimer0();
	InitTimer1();

	UART_Init(115200);

}

void InitSystick(void){

	STRELOAD = ( STCALIB) - 1;	// Recarga cada 10 ms a 100MHz
	STCURR = 0;	// Cargando con cero limpio y provoco el disparo de una intrrupcion
	STCTRL |= ((1<<ENB)|(1<<INT)|(1<<CLK));
}



