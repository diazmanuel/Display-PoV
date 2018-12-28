/**
 * @file myTasks.c
 * @brief Todas las tareas del S.O.
 * @details El codigo fuente, asi como el resto del proyecto,
 * se encuentra disponible en su totalidad, libre y gratuito en:
 *
 * https://github.com/CastroGerman/TD2-Project/
 *
 * Cualquier consulta, correccion y/o aporte al codigo y al proyecto es bienvenida por mail a:
 * gcastro@est.frba.utn.edu.ar
 *
 * Atentamente: Castro Germán.
 *
 * @author Castro Germán
 * @date 07-Dic-2018
 */

#include "Shifter.h"
#include "HallSensor.h"
#include "Display.h"
#include "Initializations.h"
#include "InterruptHandler.h"
#include "StatusHandler.h"
#include "myTimers.h"
#include "GPIOLPC1769.h"
#include "SD.h"
#include "lpc_types.h"
#include "Serial.h"

void disk_timerproc();

unsigned int Interrupt_Flags;


void InitInterruptFlags(void)
{
	Interrupt_Flags=0x00000000;
}

void SysTick_Handler(void){


	static uint8_t DiscountSteam=0;
	if (DiscountSteam==0){
		Stream();
		DiscountSteam=REFRESH_STREAM_S;
	}
	DiscountSteam--;
	disk_timerproc();	// Para el SPI/FatFS, se debe llamar c/ 10ms, cuando cierro el archivo deja de molestar

	STCTRL&=~(0x01<<FLAG);
}



void CheckForInterrupt(Shifter_t *This_Shifter,HallSensor_t *This_HallSensor)
{
	if(!((Status_Flags>>REFRESHTIMER_ALREADY_SETUP)&ON))
	{
		if(((Interrupt_Flags>>REFRESHTIMER_SETUP)&ON))
		{
			RefreshTimerSetup(This_HallSensor);
			Interrupt_Flags&=~(0x01<<REFRESHTIMER_SETUP);
		}else
		if((Interrupt_Flags>>HALLSENSOR_INT)&ON)
		{
			HallSensorInterrupt(This_HallSensor);
			Interrupt_Flags&=~(0x01<<HALLSENSOR_INT);
		}
	}else
	{

		if((Interrupt_Flags>>REFRESHDATA_READY)&ON)
		{
			CargaData(Shifter);
			Interrupt_Flags&=~(0x01<<REFRESHDATA_READY);
		}
		if((Interrupt_Flags>>DECOMPRESS)&ON)
		{
			Descomprimir();
			Interrupt_Flags&=~(0x01<<DECOMPRESS);
		}else
		{
			SerialManager ();
		}
	}
}

