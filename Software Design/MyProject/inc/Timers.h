/*
 * Timers.h
 *
 *  Created on: May 17, 2018
 *      Author: germi
 */

#ifndef TIMERS_H_
#define TIMERS_H_

#include "HallSensor.h"

//Para Timer0
#define FRECUENCIA_DE_IMPRESION	150 //[Hz]Frec de muestreo latch (50hz por cada estado de pwm = minimo para que no lo note el ojo)
#define PERIODO_LATCH	(100000000/FRECUENCIA_DE_IMPRESION) //[100MHz/Hz]

#define FILAS_IMAGEN	128
#define MS_10			1000000	//1ms a 100Mhz
#define US_1			100		//1us a 100Mhz
#define US_800			80000	//800us a 100Mhz
#define US_400			40000	//400us a 100Mhz




void InitTimer0 (void);
void InitTimer1 (void);
void TIMER0_IRQHandler(void);
void TIMER1_IRQHandler(void);
void RefreshTimerSetup(HallSensor_t *This_HallSensor);
void RefreshTimerReset(void);
#endif /* TIMERS_H_ */
