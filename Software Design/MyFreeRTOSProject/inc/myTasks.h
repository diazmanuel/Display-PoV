/**
 * @file MyTasks.h
 * @brief Asociaciones a MyTasks.c
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
#ifndef MYTASKS_H_
#define MYTASKS_H_

#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <stdlib.h>

extern xSemaphoreHandle stick, sShiftData, sHallSensor, sRefreshTimerSetup, sCargaData, sRefreshDataInterrupt, sDescomprimir;



void CrearSemaforos(void);
void CrearTareas(void);
void tPeriodic (void *pv);
void tInit(void *vp);
void tShiftData(void *pv);
void tHallSensor(void *pv);
void tRefreshTimerSetup(void *pv);
void tCargaData(void *pv);
void tRefreshDataInterrupt(void *pv);
void tDescomprimir(void *pv);

#endif /* MYTASKS_H_ */
