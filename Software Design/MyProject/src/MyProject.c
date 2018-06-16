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

	/*datinhos[0][0]=0x00000000;
	datinhos[0][1]=0x00000000;
	datinhos[0][2]=0x00000000;

	datinhos[1][0]=0x00000000;
	datinhos[1][1]=0x00000000;
	datinhos[1][2]=0x00000000;

	datinhos[2][0]=0x00000000;
	datinhos[2][1]=0x00000000;
	datinhos[2][2]=0x00000000;

	datinhos[3][0]=0x00249249; //Todos verde
	datinhos[3][1]=0x00249249;
	datinhos[3][2]=0x00249249;

	CargarShifter(Shifter,datinhos);
*/
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


/*		i++;
		if(i==500000)
				{
			datinhos[3][0]=0x00249249; //Todos verde
			datinhos[3][1]=0x00249249;
			datinhos[3][2]=0x00249249;

			CargarShifter(Shifter,datinhos);
				}
		if(i==2*500000)
				{
			datinhos[3][0]=0x00924924; //Todos verde
			datinhos[3][1]=0x00924924;
			datinhos[3][2]=0x00924924;

			CargarShifter(Shifter,datinhos);
				}
		if(i==3*500000)
		{
			i=0;
			datinhos[3][0]=0x00492492; //Todos verde
			datinhos[3][1]=0x00492492;
			datinhos[3][2]=0x00492492;

			CargarShifter(Shifter,datinhos);
				}
*/
      	CheckForInterrupt(Shifter,HallSensor);

    }
        EliminarShifter(&Shifter);
        EliminarHallSensor(&HallSensor);
return 0 ;
}
