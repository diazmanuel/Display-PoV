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

	Bluetooth_Init(38400);//tarda alrededor de 4 segundos en ejecutarse completamente
							 //debido a limitaciones del HC05 que hay que tener
							//en cuenta a la hora de RESETEARLO Y CONFIGURARLO

	UART_Init(115200);

}




