/*
 * GPIOLPC1769.h
 *
 *  Created on: May 16, 2018
 *      Author: germi
 */

#ifndef GPIOLPC1769_H_
#define GPIOLPC1769_H_

#include "RegsLPC1769.h"
void SetDIR( uint8_t ,uint8_t ,uint8_t );
void SetPIN( uint8_t , uint8_t , uint8_t );
uint8_t GetPIN( uint8_t , uint8_t , uint8_t );
void toggle (uint8_t puerto , uint8_t pin);



#endif /* GPIOLPC1769_H_ */
