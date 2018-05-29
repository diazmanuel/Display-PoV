/*
 * Shifter.h
 * 	Pinout:
 *		1	Q1		16	VCC
 *		2	Q2		15	Q0
 *		3	Q3		14	DS
 *		4	Q4		13 	-OE
 *		5	Q5		12	STCP
 *		6	Q6		11 	SHCP
 *		7	Q7		10	-MR
 *		8 	GND		9	Q7S
 *  Created on: May 19, 2018
 *      Author: germi
 */

#ifndef SHIFTER_H_
#define SHIFTER_H_

#define PWM_STATE		3
#define FILAS_SHIFTERS	4
#define CANT_SHIFTERS	3
#define DATA_SIZE		8*CANT_SHIFTERS

#define D0_PIN		0,10
#define D1_PIN		0,11
#define D2_PIN		0,12
#define D3_PIN		0,22
#define SCLK_PIN	2,13
#define LCLK_PIN	0,21

extern int ShiftCounter;
extern int PwmStateIndex;	//No puedo declararlo dentro del struct porque lo uso en interrupcion de timer 0
//Fijarse solucion si hay.

typedef struct Shifter
{
	unsigned int Data[FILAS_SHIFTERS][PWM_STATE];	//Uso 24bits de los 32 de la direccion de memoria.
													//Uso 1 posicion de memoria por fila de shifters.
	struct Shifter *DataSource;
}Shifter_t;

void InitShifter (void);
void CrearShifter(Shifter_t **ShifterHEAD);
void EliminarShifter(Shifter_t **ShifterHEAD);
void CargarShifter(Shifter_t *This_Shifter, unsigned int DataIN[FILAS_SHIFTERS][PWM_STATE]);
void ShiftDataInterrupt (Shifter_t *This_Shifter);
void RefreshDataInterrupt(Shifter_t *This_Shifter);

/*
 * Funciones de testing que deben ser eliminadas o no usadas en la version final:
 * CargarShifter (Porque la carga va a ser automatica por RefreshDataInterrupt).
 */

#endif /* SHIFTER_H_ */
