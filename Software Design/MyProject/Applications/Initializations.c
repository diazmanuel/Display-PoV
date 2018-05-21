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

/********************************************************************************
	\fn  void InitGPIOs ( void )
	\brief Inicialización de GPIO.
	\author & date: Informática II
 	\param void
	\return:	void
*/

void InitGPIOs(void)
{

}
/********************************************************************************
	\fn  void Inicializar ( void )
	\brief Inicializacion de Hardware.
	\author & date: Informática II
 	\param void
	\return:	void
*/
void Init ( void )
{
	InitPLL ( ) ;
	UART_Init(115200);

}




