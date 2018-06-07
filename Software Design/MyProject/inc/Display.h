/*
 * Display.h
 *
 *  Created on: May 17, 2018
 *      Author: germi
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "Shifter.h"



void DisplaySend(Shifter_t *This_Shifter);
void DisplayStop(void);
void PrintStart(Shifter_t *This_Shifter);
void PrintStop(void);
void CargaData(Shifter_t *This_Shifter);
void Descomprimir (void);


#endif /* DISPLAY_H_ */
