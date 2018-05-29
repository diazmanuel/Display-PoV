/*
 * InterruptHandler.h
 *
 *  Created on: May 17, 2018
 *      Author: germi
 */

#ifndef INTERRUPTHANDLER_H_
#define INTERRUPTHANDLER_H_
#include "Shifter.h"
#include "HallSensor.h"

#define ENABLE 	1
#define DISABLE	0

extern unsigned int Interrupt_Flags;
	#define		TIMER0_INT			0
	#define 	SHIFTDATA_READY		1
	#define		HALLSENSOR_INT		2
	#define 	REFRESHTIMER_SETUP	3
	#define 	REFRESHDATA_READY	4
	#define 	STORAGEDATA_READY	5
	//TIMER PWM
	//TIMER REFHRESH ANGULO
	//TIMER MOVER DATO
void InitInterruptFlags(void);
void CheckForInterrupt(Shifter_t *This_Shifter,HallSensor_t *This_HallSensor);
void SysTick_Handler(void);


#endif /* INTERRUPTHANDLER_H_ */
