/*
 * HallSensor.c
 *
 *  Created on: May 20, 2018
 *      Author: germi
 */

#include "HallSensor.h"
#include "RegsLPC1769.h"
#include "PinoutConfigLPC1769.h"
#include "GPIOLPC1769.h"
#include "InterruptHandler.h"
#include "StatusHandler.h"
#include <stdlib.h> //Malloc

unsigned int TiempoVuelta;

void InitHallSensor(void)
{
	SetPINSEL(HALL_PIN, 1); 			// Pin:P2[10] 	GPIO / EINT0
	EXTMODE |= ( 1 << EXTMODE0 );		// externa por flanco
	EXTPOLAR |= ( 1 << EXTPOLAR0 ); 	// ascendente
	ISER0 |= ( 0x01 <<18 ) ;			// Habilito Interrupcion externa 0
}

void CrearHallSensor(HallSensor_t **HallSensorHEAD)
{
	HallSensor_t *Main;
	Main = malloc(sizeof(HallSensor_t));
	Main->Next=NULL;
	*HallSensorHEAD = Main;

}
void EliminarHallSensor(HallSensor_t **HallSensorHEAD)
{
	free(*HallSensorHEAD);
}

void CargarHallSensor(HallSensor_t *This_HallSensor, unsigned int WorkingFrequencyIN)
{
	This_HallSensor->WorkingFrequency=WorkingFrequencyIN;
}
void EINT0_IRQHandler(void)
{
	TiempoVuelta=T1TC;
	/*
	 * Falta realizar cuentas para que TiempoVuelta tenga una unidad de tiempo interpretable.
	 */
	T1TC=0x00000000;	//Reinicio cuenta (Puedo hacerlo tambien por CR en T1TCR)
	EXTINT |= ( 1 << EINT0 );	// borro el flag EINT0 de interrupcion externa 0 del registro EXTINT
	Interrupt_Flags|=(ENABLE<<HALLSENSOR_INT);
}

void HallSensorInterrupt(HallSensor_t *This_HallSensor)
{
	unsigned int CotaSuperior=TiempoVuelta*(1 + 1/100);	//El 1% de la cuenta
	unsigned int CotaInferior=TiempoVuelta*(1 - 1/100);
	if(CotaInferior<<This_HallSensor->WorkingFrequency<<CotaSuperior) //Admito margen del 1%
	{
		if(!((Status_Flags>>REFRESHTIMER_ALREADY_SETUP)&&ON))	//Si NO ESTA setteado, lo setteo.
		{
			//Configuro el timer encargado del refresh.
			Interrupt_Flags|=(ENABLE<<REFRESHTIMER_SETUP);
			Status_Flags|=(ON<<REFRESHTIMER_ALREADY_SETUP);
		}
	}else{
		//Vamos promediando para tener un valor mas exacto.
		This_HallSensor->WorkingFrequency= (This_HallSensor->WorkingFrequency + TiempoVuelta)/2;
	}
}
