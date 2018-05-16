/*
===============================================================================
ConfigLPC1769.c
===============================================================================
*/
#include "PinoutConfigLPC1769.h"

/********************************************************************************
	\fn  void SetPINSEL (uint8_t puerto , uint8_t pin, uint8_t sel)
	\brief Selección de modo de los puertos (4 modos).
	\author & date: Informatica II
 	\param [in] puerto: port a configurar
 	\param [in] pin:	pin del port a configurar
 	\param [in] funcion:	selección de la funcion que tendra el pin  [0 - 3]
	\return void
*/
void SetPINSEL( uint8_t puerto , uint8_t pin ,uint8_t funcion )
{
	puerto = puerto * 2 + pin / 16;
	pin = ( pin % 16 ) * 2;
	PINSEL[ puerto ] = PINSEL[ puerto ] & ( ~ ( 3 << pin ) );
	PINSEL[ puerto ] = PINSEL[ puerto ] | ( funcion << pin );
}

/********************************************************************************
	\fn  void SetPINMODE( uint8_t port , uint8_t pin ,uint8_t modo)
	\brief Selección de modo de los puertos cuando trabajan como entradas
	\author & date: Informatica II
 	\param [in] puerto: port a configurar
 	\param [in] pin:	pin del port a configurar
 	\param [in] funcion:	selección de la funcion que tendra el pin  [0 - 3]
	\return void
*/
void SetMODE( uint8_t port , uint8_t pin ,uint8_t modo)
{
	port = port * 2 + pin / 16;
	pin = ( pin % 16 ) * 2;
	PINMODE[ port ] = PINMODE[ port ] & ( ~ ( 3 << pin ) );
	PINMODE[ port ] = PINMODE[ port ] | ( modo << pin );
}

