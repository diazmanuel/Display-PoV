/*
===============================================================================
 Name        : MyProject.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include "lz4.h"
#include "Initializations.h"
#include "Serial.h"


#include "Shifter.h"
#include "Display.h"
#include "InterruptHandler.h"
#include "HallSensor.h"
#include "StatusHandler.h"

struct Buffer Image={0};

int main (void){

	//PCONP &=~(0x01 << 2); //apago timer1
	Shifter_t *Shifter;
	HallSensor_t *HallSensor;
	CrearShifter(&Shifter);
	CrearHallSensor(&HallSensor);


	unsigned int datinhos[FILAS_SHIFTERS][PWM_STATE];

	Inicializar();

	DisplayStop();
	DisplaySend(Shifter);
int i=0;

	while(1) {
		i++;
if(i==50000)
{
	Interrupt_Flags|=(1<<REFRESHDATA_READY);
	i=0;
}
		SerialManager ();
      	CheckForInterrupt(Shifter,HallSensor);

    }
        EliminarShifter(&Shifter);
        EliminarHallSensor(&HallSensor);
return 0 ;
}
