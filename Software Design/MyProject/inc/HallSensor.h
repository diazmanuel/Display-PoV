/*
 * HallSensor.h
 *
 *  Created on: May 20, 2018
 *      Author: germi
 */

#ifndef HALLSENSOR_H_
#define HALLSENSOR_H_

#define HALL_PIN	2,10	//EINT0

extern unsigned int TiempoVuelta;

typedef struct HallSensor
{
	unsigned int WorkingFrequency;
	struct HallSensor *Next;	//En principio no lo uso. Pero parece que entiendo de programacion
								//asi que lo dejo.
}HallSensor_t;

void InitHallSensor(void);
void CrearHallSensor(HallSensor_t **HallSensorHEAD);
void EliminarHallSensor(HallSensor_t **HallSensorHEAD);
void CargarHallSensor(HallSensor_t *This_HallSensor, unsigned int WorkingFrequencyIN);
void EINT0_IRQHandler(void);
void HallSensorInterrupt(HallSensor_t *This_HallSensor);
/*
 * Funciones de testing que deben ser eliminadas o no usadas en la version final:
 * CargarHallSensor (Porque la carga va a ser automatica por el Timer1 al inicio de programa).
 */


#endif /* HALLSENSOR_H_ */
