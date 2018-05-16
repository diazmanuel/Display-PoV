/*
===============================================================================
Initializations.c
===============================================================================
*/

#include "GPIOLPC1769.h"
#include "PinoutConfigLPC1769.h"
#include "Oscilador.h"
#include "RegsLPC1769.h"


/********************************************************************************
	\fn  void InitGPIOs ( void )
	\brief Inicialización de GPIO.
	\author & date: Informática II
 	\param void
	\return:	void
*/

void InitGPIOs(void)
{
	//SetPINSEL(LEDBOARD,00);
	//SetDIR(LEDBOARD,1);


}
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
	InitGPIOs();


}




