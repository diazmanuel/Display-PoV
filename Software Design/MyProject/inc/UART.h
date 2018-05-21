/*
 * FW_Serie.h
 *
 *  Created on: 09/07/2013
 *      Author: Pablo
 */

#ifndef UART_H_
#define UART_H_

#include "RegsLPC1769.h"

void UART_Init(uint32_t baudRate);
uint8_t UART_ReceiveByte(void);
void UART_SendByte(uint8_t byte);

#endif /* UART_H_ */
