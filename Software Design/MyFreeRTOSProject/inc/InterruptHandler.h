/**
 * @file InterruptHandler.h
 * @brief Asociacion a Serial.c en myTasks.c
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
#ifndef INTERRUPTHANDLER_H_
#define INTERRUPTHANDLER_H_

#include "HallSensor.h"
#include "Shifter.h"

extern unsigned int Interrupt_Flags;

	#define HALLSENSOR_INT 		2
	#define REFRESHTIMER_SETUP 	3
	#define REFRESHDATA_READY 	5
	#define DECOMPRESS 			6


#define REFRESH_STREAM 		1				//tiempo en segundos (tiempo minimo 0.01s)
#define REFRESH_STREAM_S	REFRESH_STREAM*10

void InitInterruptFlags(void);
void CheckForInterrupt(Shifter_t *This_Shifter,HallSensor_t *This_HallSensor);

#endif /* INTERRUPTHANDLER_H_ */
