/**
 * @file GPIOLPC1769.h
 * @brief Asociaciones a GPIOLPC1769.c
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
#ifndef GPIOLPC1769_H_
#define GPIOLPC1769_H_

#include "RegsLPC1769.h"
void SetDIR( uint8_t ,uint8_t ,uint8_t );
void SetPIN( uint8_t , uint8_t , uint8_t );
uint8_t GetPIN( uint8_t , uint8_t , uint8_t );
void toggle (uint8_t puerto , uint8_t pin);


#endif /* GPIOLPC1769_H_ */
