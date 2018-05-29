/*
 * StatusHandler.c
 *
 *  Created on: May 21, 2018
 *      Author: germi
 */


#include "StatusHandler.h"

unsigned int Status_Flags;

void InitStatusFlags (void)
{
	Status_Flags=0x00000000;
}
