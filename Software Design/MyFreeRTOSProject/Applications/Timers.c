/**
 * @file Timers.c
 * @brief Funciones asociadas a los Timer 0 y 1.
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
#include "myTimers.h"
#include "InterruptHandler.h"
#include "StatusHandler.h"
#include "RegsLPC1769.h"
#include "GPIOLPC1769.h"
#include "Shifter.h"
#include "Display.h"
#include "HallSensor.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "myTasks.h"


/**
 * @fn void InitTimer0 (void)
 * @brief Inicializa Timer 0
 * @details Encargado de ser el clock de shifteo y latcheo de los C.I. Shifter.
 * Encargado de generar efecto dimmer por PWM en los leds.
 */
void InitTimer0 (void)
{
		MYPCONP |= (0x01 << 1); 	//Power Control for Peripherals registers.

		PCLKSEL0 &=~ (0x03<<2);
		PCLKSEL0 |= (0x01<<2);		//PCLK = CLK

		T0TCR &=~(1<<CE);		//paramo to2
		T0TCR |= (1<<MYCR);

		T0CTCR &=~(0x03);		//Funcion : timer.
		T0MCR &=~(0xFF);		//limpiamos por si las weas
		T0MCR |= (1<<MR0I);		//reset on mr0
		T0MCR |= (1<<MR0R);
		T0MCR |= (1<<MR1I);

		T0MR0=	PERIODO_LATCH/(8*CANT_SHIFTERS);		//PERIODO
		T0MR1=	T0MR0/2;		//Duty Cycle 50%

		ISER0|=0x02; 		//habilitamos IR
}


/**
 * @fn void InitTimer1(void)
 * @brief Inicializa Timer 1
 * @details Encargado del resfresco de los datos a mostrar.
 *	Principio de funcionamiento:
 *	Este timer arranca a contar "free wheeling" sin habilitar su interrupcion.
 *	Al iniciar el programa, tendra un tiempo de demora hasta que el motor arranque,
 * 	llegue a una velocidad de trabajo estable, la cual será sensada y calculada por HallSensor_t.
 * 	La velocidad estable será calculada en base a la cuenta que lleve este timer,
 * 	traducida y almacenada en HallSensor_t->WorkingFrequency para que a su vez,
 * 	dicha variable SOBRE ESCRIBIRA los registros T1MR0 y T1MR1 para adecuar los periodos de trabajo
 * 	a los que se debe realmente refreshear el dato (2,8º)
 *
 * 	Notese que la configuracion se realiza solamente sobre el timer 1 y no tambien sobre el timer 0,
 * 	los cuales logicamente deberian tener algun tipo de relacion.
 * 	Con esta salvedad se logra independizar la frecuencia de muestreo (asociada al shifteo + pwm)
 * 	y la frecuencia de refresco (asociada al cambio del dato a imprimir) de la imagen.
 */
void InitTimer1(void)
{

	MYPCONP |= (0x01 << 2);

	PCLKSEL0 &=~ (0x03<<4);
	PCLKSEL0 |= (0x01<<4);		//PCLK = CLK

	T1TCR &=~(1<<CE);		//paramo to2
	T1TCR |= (1<<MYCR);

	T1CTCR &=~(0x03);		//Funcion : timer.
	T1MCR &=~(0xFF);		//limpiamos por si las weas

	T1TCR &=~(1<<MYCR);
	T1TCR |=(1<<CE);
}


/**
 * @fn void TIMER0_IRQHandler(void)
 * @brief Handler de interrupcion del NVIC del Cortex-M3
 * @details  Interrupcion del Timer 0 = Timer encargado de la logica de shifteo y latcheo.
 * Que tambien se encarga de cambiar PwmStateIndex (A la frecuencia de latcheo)
 */
void TIMER0_IRQHandler(void)
{
	if( T0IR & (1 << IRMR0))// Si interrumpio Match 0
	{
		 T0IR |=(1 << IRMR0 );	// Borro flag del Match 0
		 SetPIN(SCLK_PIN,1);	// El SCLK es activo por flanco ascendente.
		 ShiftCounter++;
		 if(ShiftCounter==(8*CANT_SHIFTERS))
		 {
			 SetPIN(LCLK_PIN,0);
			 ShiftCounter=0;
			 PwmStateIndex++;
			 PwmStateIndex%=3;
		 }

			ShiftDataInterrupt(Shifter);

		 /*
		  * IMPORTANTE! LA ATENCION DE ESTA INTERRUPCION DEBE SER MAS RAPIDA QUE UN CICLO
		  * DEL SCLK PARA QUE CUANDO COMPLETE SU PERIODO, HAYA UN DATO DISPONIBLE PARA
		  * SHIFTEAR
		  */

	}
	if( T0IR & (1 << IRMR1))	// Si interrumpio Match 1
	{
		T0IR |=(1 << IRMR1 ); 	// Borro flag del Match 1
		SetPIN(SCLK_PIN,0);
		 if(ShiftCounter==0)
		 {
			 SetPIN(LCLK_PIN,1);	//Hasta aca, se ve la primer linea printeada.
			 /*
			  * Bug: El primer print se va a hacer con basura de datos previos.
			  */
		 }
	}
}

/**
 * @fn void TIMER1_IRQHandler(void)
 * @brief Handler de interrupcion del NVIC del Cortex-M3
 * @details  Ver descripcion en InitTimer1. Avisa al S.O. del cambio de datos.
 */
void TIMER1_IRQHandler(void)
{
	if(T1IR & (1<<IRMR0))
	{
		RefreshDataInterrupt(Shifter);
		T1IR|=(1<<IRMR0);
		portBASE_TYPE xTaskSwitchRequired = pdFALSE;
		xSemaphoreGiveFromISR(sCargaData, &xTaskSwitchRequired);
		portEND_SWITCHING_ISR(xTaskSwitchRequired);
	}
}

/**
 * @fn void RefreshTimerSetup(HallSensor_t *This_HallSensor)
 * @brief Configura el Timer 1 con datos del sensor hall.
 * @details Una vez detectada una frecuencia estable, re-configura el timer 1
 * con su nuevo periodo y lo habilita a interrumpir al S.O.
 * @param This_HallSensor Puntero a la informacion del sensor hall.
 */
void RefreshTimerSetup(HallSensor_t *This_HallSensor)
{
	T1TCR &=~(1<<CE);		//paramo to2
	T1TCR |= (1<<MYCR);

	T1MCR |= (1<<MR0I);		//IR, reset on mr0
	T1MCR |= (1<<MR0R);
	T1MR0=	This_HallSensor->WorkingFrequency/FILAS_IMAGEN;		//Deberia refreshear el dato cada 2.8º aprox

	ISER0|=(0x01<<2); 		//habilitamos IR

	T1TCR &=~(1<<MYCR);
	T1TCR |=(1<<CE);
	Status_Flags|=(ON<<REFRESHTIMER_ALREADY_SETUP);
}

