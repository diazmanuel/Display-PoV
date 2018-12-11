/**
 * @file myTimers.h
 * @brief Asociaciones a Timers.c
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
#ifndef TIMERS_H_
#define TIMERS_H_

#include "HallSensor.h"

/**
 * @def FRECUENCIA_DE_IMPRESION
 * Frecuencia en Hertz de muestreo. Definida por el usuario.
 */
#define FRECUENCIA_DE_IMPRESION	3300//[Hz]Frec de muestreo latch (50hz por cada estado de pwm = minimo para que no lo note el ojo)
/**
 * @def PERIODO_LATCH
 * Periodo al cual sera seteado el timer de shifteo teniendo en cuenta FRECUENCIA_DE_IMPRESION
 * y la frecuencia a la que trabaja el micro (PLL)
 */
#define PERIODO_LATCH	(100000000/FRECUENCIA_DE_IMPRESION) //[100MHz/Hz]
/**
 * @def FILAS_IMAGEN
 * Cantidad de filas en las que esta dividida la imagen de la aplicacion de PC.
 * Representa tambien la cantidad de veces que las ilera de led debe tener un dato distinto.
 */
#define FILAS_IMAGEN	128

void InitTimer0 (void);
void InitTimer1 (void);
void TIMER0_IRQHandler(void);
void TIMER1_IRQHandler(void);
void RefreshTimerSetup(HallSensor_t *This_HallSensor);
#endif /* TIMERS_H_ */
