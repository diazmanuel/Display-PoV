/*
 * flags.h
 *
 *  Created on: 11 de may. de 2018
 *      Author: Manuel
 */

#ifndef FLAGS_H_
#define FLAGS_H_

#include "RegsLPC1769.h"
#define Decompress		0
#define DataRefresh		1
#define InterruptHall	2

uint8_t ReadFlag(uint8_t pos);
void WriteFlag(uint8_t pos,uint8_t state);

#endif /* FLAGS_H_ */
