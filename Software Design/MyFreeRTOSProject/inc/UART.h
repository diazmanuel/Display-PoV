/**
 * @file UART.h
 * @brief Asociaciones a UART.c
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
#ifndef UART_H_
#define UART_H_

#include "RegsLPC1769.h"

#define TXD0_PIN	0,2
#define RXD0_PIN	0,3



void UART_Init(uint32_t baudRate);
uint8_t UART_ReceiveByte(void);
void UART_SendByte(uint8_t byte);

#endif /* UART_H_ */
