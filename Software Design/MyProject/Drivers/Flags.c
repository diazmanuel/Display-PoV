/*
 * Flags.c
 *
 *  Created on: 11 de may. de 2018
 *      Author: Manuel
 */
#include <Flags.h>



static uint32_t Flag=0;

uint8_t ReadFlag(uint8_t pos){
	return ((Flag << pos) & 0x01) ;
}


void WriteFlag(uint8_t pos,uint8_t state){

				Flag &= ~(0x01 << pos);
				Flag |= state <<pos;
}
