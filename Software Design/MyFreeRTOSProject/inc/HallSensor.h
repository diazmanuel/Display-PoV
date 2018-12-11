/**
 * @file HallSensor.h
 * @brief Asociaciones a HallSensor.c
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
#ifndef HALLSENSOR_H_
#define HALLSENSOR_H_

/**
 * @def HALL_PIN
 * <Puerto,Pin> de entrada del microcontrolador al que irá conectado la señal analogica del hall sensor.
 * Fue dispuesta para la interrupcion externa 1 (EINT1).
 */
#define HALL_PIN	2,11	//EINT1
/**
 * @def ISE_EINT1
 * Bit asociado a la EINT1 en el Interrupt Set-Enable Register 0 del microcontrolador
 */
#define ISE_EINT1	19

/**
 * @struct HallSensor
 * @brief HallSensor data struct
 *
 * Contiene toda la informacion necesaria para saber a que frecuencia de giro trabaja el motor.
 */
typedef struct HallSensor
{
	int WorkingFrequency; /**< Contiene la frecuencia de trabajo del motor*/
	int TiempoVuelta;	/**< Contiene el tiempo real que tardó en dar una vuelta el motor*/
}HallSensor_t;

extern HallSensor_t *HallSensor;
extern int PhaseRefresh;

void InitHallSensor(void);
void CrearHallSensor(HallSensor_t **HallSensorHEAD);
void EliminarHallSensor(HallSensor_t **HallSensorHEAD);
void EINT0_IRQHandler(void);
void HallSensorInterrupt(HallSensor_t *This_HallSensor);


#endif /* HALLSENSOR_H_ */
