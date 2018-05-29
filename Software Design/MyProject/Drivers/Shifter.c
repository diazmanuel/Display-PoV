/*
 * Shifter.c
 *
 *  Created on: May 19, 2018
 *      Author: germi
 */

#include "Shifter.h"
#include "GPIOLPC1769.h"
#include "PinoutConfigLPC1769.h"
#include "InterruptHandler.h"
#include <stdlib.h> //Malloc

int ShiftCounter;
int	PwmStateIndex;

void InitShifter (void)
{
	SetPINSEL(D0_PIN,0);
	SetPINSEL(D1_PIN,0);
	SetPINSEL(D2_PIN,0);
	SetPINSEL(D3_PIN,0);
	SetPINSEL(SCLK_PIN,0);
	SetPINSEL(LCLK_PIN,0);

	SetMODE(D0_PIN,01);
	SetMODE(D1_PIN,01);
	SetMODE(D2_PIN,01);
	SetMODE(D3_PIN,01);
	SetMODE(SCLK_PIN,01);
	SetMODE(LCLK_PIN,01);

	SetDIR(D0_PIN,1);
	SetDIR(D1_PIN,1);
	SetDIR(D2_PIN,1);
	SetDIR(D3_PIN,1);
	SetDIR(SCLK_PIN,1);
	SetDIR(LCLK_PIN,1);
}

/*
 * Ya me aseguro los dos slots en memoria (No printeo error por consola)
 */
void CrearShifter(Shifter_t **ShifterHEAD)
{
	Shifter_t *Main, *Source;
	Main = malloc(sizeof(Shifter_t));
	Source = malloc(sizeof(Shifter_t));
	Source->DataSource = NULL;
	*ShifterHEAD = Main;
	(*ShifterHEAD)->DataSource = Source;
}
void EliminarShifter(Shifter_t **ShifterHEAD)
{
	free((*ShifterHEAD)->DataSource);
	free(*ShifterHEAD);
}


void CargarShifter(Shifter_t *This_Shifter, unsigned int DataIN[FILAS_SHIFTERS][PWM_STATE])
{
	for(int i=0;i<FILAS_SHIFTERS;i++)
	{
		for(int j=0;j<PWM_STATE;j++)
		{
			This_Shifter->Data[i][j]=DataIN[i][j];
		}
	}
}

void ShiftDataInterrupt (Shifter_t *This_Shifter)//Shifteo del LSB al MSB
{
	SetPIN(D0_PIN,(This_Shifter->Data[0][PwmStateIndex]>>ShiftCounter)&1);
	SetPIN(D1_PIN,(This_Shifter->Data[1][PwmStateIndex]>>ShiftCounter)&1);
	SetPIN(D2_PIN,(This_Shifter->Data[2][PwmStateIndex]>>ShiftCounter)&1);
	SetPIN(D3_PIN,(This_Shifter->Data[3][PwmStateIndex]>>ShiftCounter)&1);
}

/*
 * RefreshDataInterrupt
 * La dispara un timer cuyo periodo fue previamente seteado por el HALL
 * Encargada de cargar el buffer de la aplicacion en un Shifter_t.
 *
 * DEBE SER LA FUNCION MAS RAPIDA DEL LEJANO OESTE.
 */
void RefreshDataInterrupt(Shifter_t *This_Shifter)
{
	for(int i=0;i<FILAS_SHIFTERS;i++)
	{
		for(int j=0;j<PWM_STATE;j++)
		{
			This_Shifter->Data[i][j]=This_Shifter->DataSource->Data[i][j];
			/*
			 * Deberia verse un error justo en el cambio de imagen por no reiniciar el ShiftCounter.
			 * Deberia arreglarse en la segunda impresion.
			 */
		}
	}
	Interrupt_Flags|=(ENABLE<<STORAGEDATA_READY);

}

