 /**
 * @file Shifter.c
 * @brief Funcionalidades de los C.I. Shifter
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
#include "GPIOLPC1769.h"
#include "PinoutConfigLPC1769.h"
#include "InterruptHandler.h"
#include "stdlib.h"


int ShiftCounter; /**< Index de shifteo*/
int	PwmStateIndex; /**< Index de estado de PWM*/

/**
 * @var Shifter
 * @brief Global Pointer to a Shifter_t
 *
 * El puntero principal que maneja la info de los shifters.
 */
Shifter_t *Shifter;

/**
 * @fn void InitShifter (void)
 * @brief Inicializa el hardware asociado a los shifters.
 * @return void
 */
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
/**
 * @fn void CrearShifter(Shifter_t **ShifterHEAD)
 * @brief Crea y aloja en RAM estatica la memoria necesaria.
 * @param ShifterHEAD: Direccion del puntero
 * @return void
 *
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
/**
 * @fn void EliminarShifter(Shifter_t **ShifterHEAD)
 * @brief Elimina la memoria previamente alojada.
 * @param ShifterHEAD: Direccion del puntero
 * @return void
 */
void EliminarShifter(Shifter_t **ShifterHEAD)
{
	free((*ShifterHEAD)->DataSource);
	free(*ShifterHEAD);
}

/**
 * @fn void ShiftDataInterrupt (Shifter_t *This_Shifter)
 * @brief Transfiere la informacion en memoria a los pines fisicos.
 * @param This_Shifter: Direccion de memoria de la informacion a transferir.
 * @return void
 */
void ShiftDataInterrupt (Shifter_t *This_Shifter)//Shifteo del LSB al MSB
{
	SetPIN(D0_PIN,(This_Shifter->Data[0][PwmStateIndex]>>ShiftCounter)&1);
	SetPIN(D1_PIN,(This_Shifter->Data[1][PwmStateIndex]>>ShiftCounter)&1);
	SetPIN(D2_PIN,(This_Shifter->Data[2][PwmStateIndex]>>ShiftCounter)&1);
	SetPIN(D3_PIN,(This_Shifter->Data[3][PwmStateIndex]>>ShiftCounter)&1);
}

/**
 * @fn void RefreshDataInterrupt(Shifter_t *This_Shifter)
 * @brief Actualiza el dato a imprimir desde el dato futuro almacenado.
 * @details La dispara un timer cuyo periodo fue previamente seteado por el sensor hall.
 * Encargada de copiar el buffer que la aplicacion cargo en un Shifter_t.
 * @param This_Shifter: Puntero a la informacion de los shifters.
 * @return void
 */
void RefreshDataInterrupt(Shifter_t *This_Shifter)
{
	for(int i=0;i<FILAS_SHIFTERS;i++)
	{
		for(int j=0;j<PWM_STATE;j++)
		{
			This_Shifter->Data[i][j]= ~(This_Shifter->DataSource->Data[i][j]);
			/*
			 * Deberia verse un error justo en el cambio de imagen por no reiniciar el ShiftCounter.
			 * Deberia arreglarse en la segunda impresion.
			 */
			This_Shifter->DataSource->Data[i][j]=0;
		}
	}
}

