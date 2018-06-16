/*
 * Display.c
 *
 *  Created on: May 17, 2018
 *      Author: germi
 */

#include "Display.h"
#include "RegsLPC1769.h"
#include "Timers.h"
#include "GPIOLPC1769.h"
#include "Shifter.h"
#include "HallSensor.h"
#include "InterruptHandler.h"
#include "Serial.h"




void DisplaySend(Shifter_t *This_Shifter)
{

	PrintStart(This_Shifter);
}

void DisplayStop(void)
{
	PrintStop();
}

void PrintStart(Shifter_t *This_Shifter)
{
	PwmStateIndex=0;
	SetPIN(D0_PIN,(This_Shifter->Data[0][PwmStateIndex])&1);	//Pongo el LSB en el pin, y arranco la secuencia.
	SetPIN(D1_PIN,(This_Shifter->Data[1][PwmStateIndex])&1);
	SetPIN(D2_PIN,(This_Shifter->Data[2][PwmStateIndex])&1);
	SetPIN(D3_PIN,(This_Shifter->Data[3][PwmStateIndex])&1);
	T0TCR &=~ (1<<CR);//Arranco timer 0 (Shifter clock)
	T0TCR |= (1<<CE);
}

void PrintStop(void)
{
	T0TCR &=~(1<<CE);	//Paro timer 0 (shifter clock)
	T0TCR |= (1<<CR);
	ShiftCounter=0;

}
/*
void CargaData(Shifter_t *This_Shifter){

	static uint8_t IndexFila=0;
	uint8_t N_Color,N_FilaShifter,N_LED,N_PWM;
	for(N_FilaShifter=0;N_FilaShifter<FILAS_SHIFTERS;N_FilaShifter++){
		for(N_PWM=0;N_PWM<PWM_STATE;N_PWM++){
			This_Shifter->DataSource->Data[N_FilaShifter][N_PWM]=0;
			for(N_LED=0;N_LED<8;N_LED++){
				for(N_Color=0;N_Color<3;N_Color++){
					This_Shifter->DataSource->Data[N_FilaShifter][N_PWM]|=(((Image.Buffers[IR][(IndexFila*32)+(N_FilaShifter*8)+N_LED]<<(N_Color*2) )&0x03)<=N_PWM ? 0x00 : 0x01<<((N_LED*3)+N_Color) );
				}
			}
		}
	}

	//23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
	// B  G  R  B  G  R  B  G  R  B  G  R  B  G  R  B  G  R  B  G  R  B  G  R
	IndexFila++;
	if(IndexFila==128){
		IndexFila=0;
	}
}*/
/**
 * CargaData
 * Aplica una transformacion a los datos recibidos descomprimidos de la aplicacion de PC
 * en un tipo de dato unsigned int y aplica logica de ordenamiento para dejar los datos
 * listos para ser interpretados por el hardware en un Shifter_t.
 * @param This_Shifter
 */
void CargaData(Shifter_t *This_Shifter){
	static uint8_t IndexFila=0;
	uint8_t N_Color,N_FilaShifter,N_Led,N_PWM;
	uint8_t Intensidad, LedInfo;
	for(N_FilaShifter=0;N_FilaShifter<FILAS_SHIFTERS;N_FilaShifter++){ 	//Primero recorro que fila de shifter es
		for(N_Led=0;N_Led<(N_LEDS/FILAS_SHIFTERS);N_Led++){				//Luego recorro todos los Leds de c/fila
			LedInfo=Image.Buffers[IR][(IndexFila*N_LEDS)+(N_FilaShifter*(N_LEDS/FILAS_SHIFTERS))+N_Led];
			for(N_Color=0;N_Color<3;N_Color++){							//Luego recorro todos los colores de c/Led (R, G y B)
				/*
				 * Por como esta ordenado el Buffer descomprimido: (R->G->B)
				 * Es decir cuando 	N_Color=0 => Informacion de Rojo
				 * 					N_Color=1 => Informacion de Verde
				 * 					N_Color=2 => Informacion de Azul.
				 */
				Intensidad = (LedInfo>>N_Color*2)&0x03; //Evaluo la intensidad de cada color para cargar el PWM. Toma valores de 0-3
				for(N_PWM=0;N_PWM<Intensidad;N_PWM++){	//Por ultimo recorro cada estado de PWM cargando la cantidad de intensidad necesaria.
					/*
					 * Por como esta ordenado el Shifter_t->Data: (G->B->R)
					 * Cuando		N_Color=0 => Informacion del Verde
					 * 				N_Color=1 => Informacion del Azul
					 * 				N_Color=2 => Informacion del Rojo
					 */
					This_Shifter->DataSource->Data[N_FilaShifter][N_PWM]|=(1<<(((N_Color+2)%3)+N_Led*3));
					/*
					 * DataSource->Data debe estar limpio antes de este llamado.
					 * (Realizado en RefreshDataInterrupt)
					 * (N_Color+2)%3 realiza la transformacion necesaria para el ordenamiento de colores de un buffer a otro.
					 */
				}
			}
		}
	}

	IndexFila++;
	IndexFila%=N_CIR;
}

/*
 * Image.Buffers[2][4096] (Peina para adentro, arranca en la ultima fila dessde la derecha)
 *
 * Ejemplo
 * Blanco 63
 * Rojo	3
 * Azul 48
 * Verde 12
 * Octal precede 0
 *
 *
 * LED Display		Ultimo											Primero(centro)
 * image.buffer: 	0	1	2	3	4	5	6	7	8	9	10	...	31
 *					B	R	A	V	B	R	A	V
 * DataSource:   0	1	2	4	7	1	2	4	7	(OCTAL)	(PWM 0) 0x002A72A7 (HEXA)
 * 					1	2	4	7	1	2	4	7			(PWM 1)
 *  				1	2	4	7	1	2	4	7			(PWM 2)
 *
 * La trama quedo armada de la siguiente manera:
 * LED					0	1	2	3	4	5	6	7	8	9	10			...					31
 * Shifters Asociados	|------FilaShifter 0--------|	|----Shifter 1 --	...	--Fila_Shifter 3-|
 * Leds de fila			0	1	2	3	4	5	6	7	0	1	2			...					7
 *
 * Si quisieras leer el dato de una fila de 32 leds de corrido. Tendrias que ordenar los datos de la siguiente forma
 *
 * 	Shifter->Data[3] Shifter->Data[2] Shifter->Data[1] Shifter->Data[0]
 * 	Si lo leyeras en este orden, estarias leyendo una fila de led desde el centro hacia afuera.
 */
void Descomprimir (void)
{
	   LZ4_decompress_safe_usingDict(
	               (const char*)Image.Stream_Buffer + 10,
	               (char*)Image.Buffers[IW],
	               Image.Stream_Size - 10,
	               N_PIX,
	               (const char*)Image.Buffers[IR],
	               N_PIX
	           );
     Image.Buffer_Index = (Image.Buffer_Index + 1) % 2;

	  Serial_PushTx('1'); //para stream
}

