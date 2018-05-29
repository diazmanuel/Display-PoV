/*
===============================================================================
GPIOLPC1769.c

===============================================================================
*/
#include "GPIOLPC1769.h"


/********************************************************************************
	\fn  void void SetDIR( uint8_t puerto , uint8_t pin , uint8_t dir )
	\brief Establece si un determinado PIN de un determinado PUERTO (previamente
			configurado como GPIO) es entrada o salida.
	\author & date: Informatica II
 	\param [in] puerto: 	puerto con el que se va a trabajar
 	\param [in] pin:		pin a configurar sentido
 	\param [in] dir:		Direccion 0 = entrada - 1 = salida.
	\return void
*/
void SetDIR( uint8_t puerto , uint8_t pin , uint8_t dir )
{
	puerto = puerto * 8;

	GPIO[ puerto ] = GPIO[ puerto ] & ( ~ ( 1 << pin ) );
	GPIO[ puerto ] = GPIO[ puerto ] | ( dir << pin );
}

/********************************************************************************
	\fn  void SetPIN( uint8_t puerto , uint8_t pin , uint8_t estado )
	\brief Establece un ESTADO en un determinado PIN de un determinado PUERTO.
	\author & date: Informatica II
 	\param [in] puerto: puerto con el que se va a trabajar
 	\param [in] pin:	pin a activar estado
 	\param [in] estado:	valor a establecer en el PIN del PUERTO [0-1].
	\return void
*/
void SetPIN( uint8_t puerto , uint8_t pin , uint8_t estado )
{
	puerto = puerto * 8 + 5;

	GPIO[ puerto ] = GPIO[ puerto ] & ( ~ ( 1 << pin ) );
	GPIO[ puerto ] = GPIO[ puerto ] | ( estado << pin );
}

/********************************************************************************
	\fn  uint8_t GetPIN( uint8_t puerto , uint8_t pin , uint8_t actividad )
	\brief Devuelve el ESTADO de un determinado PIN de un determinado PUERTO.
	\author & date: Informatica II
 	\param [in] puerto: puerto con el que se va a trabajar
 	\param [in] pin:	pin a consultar estado
	\param [in] actividad:	ALTO = 1, BAJO = 0
	\return:	estado del pin consultado (uint_8)
*/
uint8_t GetPIN( uint8_t puerto , uint8_t pin , uint8_t actividad )
{
	puerto = puerto * 8 + 5;

	return ( ( ( GPIO[ puerto ] >> pin ) & 1 ) == actividad ) ? 1 : 0;

}

void toggle (uint8_t puerto , uint8_t pin){

	if(GetPIN(puerto,pin,1)){
		SetPIN(puerto,pin,0);
	}else{
		SetPIN(puerto,pin,1);
	}
}


void InitGPIOs(void){

}





