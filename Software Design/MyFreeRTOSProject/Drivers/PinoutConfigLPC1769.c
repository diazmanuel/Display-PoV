/**
 * @file PinoutConfigLPC1769.c
 * @brief Manejo de GPIO
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
	MYPINSEL[ puerto ] = MYPINSEL[ puerto ] & ( ~ ( 3 << pin ) );
	MYPINSEL[ puerto ] = MYPINSEL[ puerto ] | ( funcion << pin );
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
	MYPINMODE[ port ] = MYPINMODE[ port ] & ( ~ ( 3 << pin ) );
	MYPINMODE[ port ] = MYPINMODE[ port ] | ( modo << pin );
}

