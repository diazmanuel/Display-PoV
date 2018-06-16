/*
 * InterruptHandler.c
 *
 *  Created on: May 17, 2018
 *      Author: germi
 */

#include "InterruptHandler.h"
#include "Display.h"
#include "GPIOLPC1769.h"
#include "Timers.h"
#include "Shifter.h"
#include "HallSensor.h"
#include "Display.h"
#include "StatusHandler.h"

unsigned int Interrupt_Flags;


void InitInterruptFlags(void)
{
	Interrupt_Flags=0x00000000;
}

void CheckForInterrupt(Shifter_t *This_Shifter,HallSensor_t *This_HallSensor)
{
	if((Interrupt_Flags>>SHIFTDATA_READY)&&ENABLE)
	{
		ShiftDataInterrupt(This_Shifter);
		Interrupt_Flags&=~(0x01<<SHIFTDATA_READY);		//limpio flag
	}
	if((Interrupt_Flags>>HALLSENSOR_INT)&&ENABLE)
	{
		HallSensorInterrupt(This_HallSensor);
		Interrupt_Flags&=~(0x01<<HALLSENSOR_INT);		//limpio flag
	}
	if((Interrupt_Flags>>REFRESHTIMER_SETUP)&&ENABLE)
	{
		//RefreshTimerSetup(This_HallSensor);
		Interrupt_Flags&=~(0x01<<REFRESHTIMER_SETUP);		//limpio flag
	}
	if((Interrupt_Flags>>STORAGEDATA_READY)&&ENABLE)
	{
		CargaData(This_Shifter);
		Interrupt_Flags&=~(0x01<<STORAGEDATA_READY);		//limpio flag
	}
	if((Interrupt_Flags>>REFRESHDATA_READY)&&ENABLE)
	{
		RefreshDataInterrupt(This_Shifter);
		Interrupt_Flags&=~(0x01<<REFRESHDATA_READY);		//limpio flag
	}
	if((Interrupt_Flags>>DECOMPRESS)&&ENABLE)
	{
		Descomprimir();
		Interrupt_Flags&=~(0x01<<DECOMPRESS);		//limpio flag
	}


}



void SysTick_Handler(void)
{
	static uint8_t i=0;
	if (i==0){
		Stream();
		i=REFRESH_STREAM_S;
	}
	i--;
}
