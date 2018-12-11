/**
 * @file UART.c
 * @brief Funcionalidades de la comunicacion UART.
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
 * @author Pablo
 * @date 07-Dic-2018
 */
#include "GPIOLPC1769.h"
#include "Serial.h"
#include "UART.h"
#include "PinoutConfigLPC1769.h"
#include "Bluetooth.h"

void UART_Init(uint32_t BaudRate)
{


	int ClockDiv=4;
	int DIVADDVAL=10;
	int MULVAL=13;
	int DLL=92;
	int DLM=0;


	switch (BaudRate){
		case 4800:
			ClockDiv=1;
			DIVADDVAL=3;
			MULVAL=8;
			DLL=179;
			DLM=3;
		break;
		case 9600:
			ClockDiv=1;
			DIVADDVAL=3;
			MULVAL=13;
			DLL=17;
			DLM=2;
		break;
		case 19200:
			ClockDiv=1;
			DIVADDVAL=10;
			MULVAL=13;
			DLL=184;
			DLM=0;
		break;
		case 13800:
			ClockDiv=1;
			DIVADDVAL=10;
			MULVAL=13;
			DLL=92;
			DLM=0;
		break;
		case 57600:
			ClockDiv=1;
			DIVADDVAL=1;
			MULVAL=6;
			DLL=93;
			DLM=0;
		break;
		case 115200:
			ClockDiv=1;
			DIVADDVAL=3;
			MULVAL=4;
			DLL=31;
			DLM=0;
		break;
		case 234000:
			ClockDiv=1;
			DIVADDVAL=3;
			MULVAL=14;
			DLL=22;
			DLM=0;
		break;
		case 460800:
			ClockDiv=1;
			DIVADDVAL=5;
			MULVAL=14;
			DLL=10;
			DLM=0;
		break;
		case 921600:
			ClockDiv=1;
			DIVADDVAL=5;
			MULVAL=14;
			DLL=5;
			DLM=0;
		break;
		case 1382400:
			ClockDiv=1;
			DIVADDVAL=2;
			MULVAL=15;
			DLL=4;
			DLM=0;
		break;

	}


	//1.- Registro PCONP - bit 3 en 1 prende la UART0:
	MYPCONP |= 0x01<<3;
	//2.- Registro PCLKSEL0 - bits 8 y 9 en 0 seleccionan que el clk de la UART0 sea 25MHz:
	PCLKSEL0 &= ~(0x03<<6);
	switch(ClockDiv){
					case 1:
						PCLKSEL0 |= (0x01<<6);
						break;
					case 2:
						PCLKSEL0 |= (0x02<<6);
						break;
					case 4:
						PCLKSEL0 |= (0x00<<6);
						break;
					case 8:
						PCLKSEL0 |= (0x03<<6);
						break;
	}
	//3.- Registro U0LCR - transmision de 8 bits, 1 bit de stop, sin paridad, sin break cond, DLAB = 1:
	U0LCR = 0x00000083;
	//4.- Registros U1DLL y U1DLM - calculo el divisor para el baud rate:
	/*
	aux = (100000000/4)/16;
	if(aux % baudRate >= baudRate/2)	//si el resto es mayor a la mitad del divisor
		aux = aux / baudRate + 1;		//redondeo para arriba
	else
		aux = aux / baudRate;
	U0DLM = (aux>>8)&0xFF;
	U0DLL = (aux>>0)&0xFF;
	*/
	U0DLM = (DLM)&0xFF;
	U0DLL = (DLL)&0xFF;
	U0FDR = DIVADDVAL;
	U0FDR |= (MULVAL<<4);

	//5.- Registros PINSEL - habilitan las funciones especiales de los pines:
	SetPINSEL(TXD0_PIN,1);
	SetPINSEL(RXD0_PIN,1);
	TestPointInitBT();
	//6.- Registro U0LCR, pongo DLAB en 0:
	U0LCR = 0x03;
	//7. Habilito las interrupciones (En la UART -IER- y en el NVIC -ISER)
	U0IER = 0x03;
	ISER0 |= (1<<5);
}

uint8_t UART_ReceiveByte(void)
{
	return U0RBR;
}

void UART_SendByte(uint8_t byte)
{
	U0THR = byte;
}

void UART0_IRQHandler(void)
{
	uint8_t iir;

	do
	{
		//IIR es reset por HW, una vez que lo lei se resetea.
		iir = U0IIR;

		if ( iir & 0x02 ) //THRE
		{
			Serial_TxIntHandler();
		}
		if ( iir & 0x04 ) //Data ready
		{
			Serial_RxIntHandler();
		}

	}
	while( ! ( iir & 0x01 ) ); /* me fijo si cuando entre a la ISR habia otra
						     	int. pendiente de atencion: b0=1 (ocurre unicamente si dentro del mismo
								espacio temporal lleguan dos interrupciones a la vez) */
}

