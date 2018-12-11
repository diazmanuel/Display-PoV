/**
 * @file Display.h
 * @brief Asociaciones a Display.c
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
#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "Shifter.h"
#include "Serial.h"

//Hardware Test Points Listed Below.
#define TPGPIO1	0,18
#define TPGPIO2	0,17
#define TPGPIO3	0,15
#define TPGPIO4	0,16
#define TPGPIO5	2,9


void DisplaySend(Shifter_t *This_Shifter);
void DisplayStop(void);
void PrintStart(Shifter_t *This_Shifter);
void PrintStop(void);
void CargaData(Shifter_t *This_Shifter);
void Descomprimir (void);
void Stream(void);
void TestPointInitGeneral(void);

#endif /* DISPLAY_H_ */
