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
	uint8_t N_Color,N_FilaShifter,N_LED,N_PWM;
	for(N_FilaShifter=0;N_FilaShifter<4;N_FilaShifter++){
		for(N_PWM=0;N_PWM<3;N_PWM++){
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
