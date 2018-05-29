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
#include "Flags.h"

#include "Shifter.h"
#include "Display.h"
#include "InterruptHandler.h"
#include "HallSensor.h"
#include "StatusHandler.h"



struct Buffer Image={0};
static uint8_t IndexFila=0;

int main(void)
{


	Shifter_t *Shifter;
	HallSensor_t *HallSensor;
	CrearShifter(&Shifter);
	CrearHallSensor(&HallSensor);


	unsigned int datinhos[FILAS_SHIFTERS][PWM_STATE];

	datinhos[0][0]=0x00000000;
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

	Inicializar();

	DisplayStop();
	DisplaySend(Shifter);
	volatile static int i = 0 ;
    while(1) {


        if(ReadFlag(Decompress)){
     	   LZ4_decompress_safe_usingDict(
     	               (const char*)Image.Stream_Buffer + 10,
     	               (char*)Image.Buffers[IW],
     	               Image.Stream_Size - 10,
     	               N_PIX,
     	               (const char*)Image.Buffers[IR],
     	               N_PIX
     	           );
            Image.Buffer_Index = (Image.Buffer_Index + 1) % 2;
     	   WriteFlag(Decompress,0);
     	   //Serial_PushTx('1'); //para stream
        }else{
     	   SerialManager ();
        }


    	CheckForInterrupt(Shifter,HallSensor);

    }
        EliminarShifter(&Shifter);
        EliminarHallSensor(&HallSensor);

return 0 ;
}
