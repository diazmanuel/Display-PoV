/*
 * PR_Serie.h
 *
 *  Created on: 09/07/2013
 *      Author: Pablo
 */

#ifndef PR_SERIE_H_
#define PR_SERIE_H_

#include "UART.h"
#include "lz4.h"

#define TXBUFFER_SIZE	60
#define RXBUFFER_SIZE	60
#define CANT 3

extern char flag;

uint8_t Serial_PushTx(uint8_t dato);
uint8_t Serial_PopRx(uint8_t *dato);
uint8_t Serial_Send(const uint8_t *buffer, uint8_t count);
void Serial_RxIntHandler(void);
void Serial_TxIntHandler(void);

void SerialManager (void);
#define FILAS_SHIFTERS 4
#define PWM_STATE 3
#define N_LEDS 32
#define N_CIR N_LEDS*4
#define N_PIX N_LEDS * N_CIR
#define IW (Image.Buffer_Index)
#define IR ((Image.Buffer_Index + 1) % 2)

struct Buffer
{
    uint8_t 	Buffers[2][N_PIX];
    uint8_t 	Stream_Buffer[LZ4_COMPRESSBOUND(N_PIX)];
    uint8_t		Buffer_Index;
    uint16_t	Stream_Size;
    uint32_t	Data[FILAS_SHIFTERS][PWM_STATE];
};

extern struct Buffer Image;
#endif /* PR_SERIE_H_ */
