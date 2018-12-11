/**
 * @file Shifter.h
 * @brief Asociaciones a Shifter.c
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
 *      Author: Germán
 */

#ifndef SHIFTER_H_
#define SHIFTER_H_

/**
 * @def PWM_STATE
 * Indice de niveles para lograr efecto dimmer con PWM en los leds. Actualmente 3 niveles.
 */
#define PWM_STATE		3
/**
 * @def CANT_SHIFTERS
 * Cantidad de shifters por fila.
 */
#define CANT_SHIFTERS	3
/**
 * @def FILAS_SHIFTERS
 * Cantidad de filas de shifters
 */
#define FILAS_SHIFTERS	4
/**
 * @def DATA_SIZE
 * Longitud del dato que se debe shiftear para completar informacion en una fila de shifters (8 leds)
 */
#define DATA_SIZE		8*CANT_SHIFTERS

/**
 * @def D0_PIN
 * <Puerto,Pin> de salida del microcontrolador y entrada a la primer fila de shifters.
 */
#define D0_PIN		1,20
/**
 * @def D1_PIN
 * <Puerto,Pin> de salida del microcontrolador y entrada a la segunda fila de shifters.
 */
#define D1_PIN		1,21
/**
 * @def D2_PIN
 * <Puerto,Pin> de salida del microcontrolador y entrada a la tercera fila de shifters.
 */
#define D2_PIN		1,22
/**
 * @def D3_PIN
 * <Puerto,Pin> de salida del microcontrolador y entrada a la cuarta fila de shifters.
 */
#define D3_PIN		1,23
/**
 * @def SCLK_PIN
 * <Puerto,Pin> de salida del microcontrolador y señal de clock para el shifteo de los datos.
 */
#define SCLK_PIN	1,25
/**
 * @def LCLK_PIN
 * <Puerto,Pin> de salida del microcontrolador y señal de clock para el latcheo de los datos.
 */

#define LCLK_PIN	1,24

extern int ShiftCounter;
extern int PwmStateIndex;	//Prefiero no declararlo dentro del struct porque lo uso en interrupcion de timer 0


/**
 * @struct	Shifter
 * @brief	Shifter data struct.
 *
 * En esta estructura se almacenan los datos listos a ser impresos en el display.
 * Consta de una lista simplemente enlazada que apunta hacia el dato futuro,
 * sobre el cual se almacena y procesa la siguiente linea de la imagen que deberá imprimirse.
 */
typedef struct Shifter
{
	/**
	 * @var	Data
	 * @brief	Informacion de una fila de leds
	 *
	 * Matriz del dato de fila de la imagen.
	 * Cada elemento de la matriz representa el dato que necesita una fila de circuitos integrados shifter, compuesta por 3 de los mismos, los cuales en conjunto controlan 8 leds RGB.
	 * Por lo tanto cada elemento de la matriz constara de de 24 bits util de los 32 que dispone.
	 */
	unsigned int Data[FILAS_SHIFTERS][PWM_STATE];
	struct Shifter *DataSource; /**< Puntero al dato futuro*/
}Shifter_t;


extern Shifter_t *Shifter;

void InitShifter (void);
void CrearShifter(Shifter_t **ShifterHEAD);
void EliminarShifter(Shifter_t **ShifterHEAD);
void ShiftDataInterrupt (Shifter_t *This_Shifter);
void RefreshDataInterrupt(Shifter_t *This_Shifter);

#endif /* SHIFTER_H_ */
