/**
  	\file PR_serie.c
 	\brief Primitiva de comunicación serie
 	\details buffers de RX y TX protegidos
 	\author Pablo Irrera Condines
 	\date 2013.05.08
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Serial.h>
#include "InterruptHandler.h"
#include "StatusHandler.h"

#define	HEADER	0
#define DATA	1


// Buffer de Transmision
uint8_t g_txBuffer[TXBUFFER_SIZE];
// Buffer de Recepcion
uint8_t g_rxBuffer[RXBUFFER_SIZE];

//Indices de Transmision
uint8_t g_txIn,g_txOut;
//Indices de Recepcion
uint8_t g_rxIn,g_rxOut;
//Espacio utilizado en los buffers
uint8_t g_txBufferCount = 0, g_rxBufferCount = 0;

//Flag de transmision en proceso
static uint8_t g_txStart = 0;

static uint8_t PushRx(uint8_t data);
static uint8_t PopTx(uint8_t *data);

static uint8_t PushRx(uint8_t data)
{
	if(g_rxBufferCount == RXBUFFER_SIZE)
		return 1;

	g_rxBuffer[g_rxIn] = data;
	g_rxIn++;
	g_rxIn %= RXBUFFER_SIZE;
	g_rxBufferCount++;

	return 0;	//dato agregado al buffer
}

static uint8_t PopTx(uint8_t *data)
{
	if(g_txBufferCount == 0)
		return 1;

	*data = (uint8_t) g_txBuffer[g_txOut];
	g_txOut++;
	g_txOut %= TXBUFFER_SIZE;
	g_txBufferCount--;

	return 0;	//dato sacado del buffer
}


void Serial_RxIntHandler(void)
{
	uint8_t byte;
	byte = UART_ReceiveByte();
	PushRx(byte);
}

void Serial_TxIntHandler(void)
{
	uint8_t byte;
	if(!PopTx(&byte))
		UART_SendByte(byte);
	else
	{
		g_txStart = 0;
	}
}

uint8_t Serial_PopRx(uint8_t *data)
{
	if(g_rxBufferCount == 0)
		return 1;

	*data = (uint8_t) g_rxBuffer[g_rxOut];
	g_rxOut++;
	g_rxOut %= RXBUFFER_SIZE;
	g_rxBufferCount--;

	return 0;	//dato sacado del buffer
}

uint8_t Serial_PushTx(uint8_t data)
{
	if(g_txBufferCount == TXBUFFER_SIZE)
		return 1;

	g_txBuffer[g_txIn] = data;
	g_txIn++;
	g_txIn %= TXBUFFER_SIZE;
	g_txBufferCount++;

	if (g_txStart == 0)
	{
		g_txStart = 1;
		Serial_TxIntHandler();
	}
	return 0;	//dato agregado al buffer
}





/**
 * Toma los datos del bluetooth
 */
void SerialManager (void){
	static int i=0;
	static int k=0;
	static uint8_t ESTADO = HEADER;

	uint8_t Dato;
if(Serial_PopRx(&Dato) == 0){
	Status_Flags&=~(0x01<<STREAM);
	switch (ESTADO){
		case HEADER:

			Image.Stream_Buffer[i]=Dato;
				i++;
				if( i==11){
					ESTADO=DATA;
					Image.Stream_Size = *(uint32_t*)(Image.Stream_Buffer + 6) + 10;
				}

		break;

		case DATA:

			Image.Stream_Buffer[i]=Dato;
			i++;
				if (i==(Image.Stream_Size)){
					i=0;
					k++;
					ESTADO=HEADER;
          
					Interrupt_Flags|=(ENABLE<<DECOMPRESS);
				}

		break;
}
}
}

