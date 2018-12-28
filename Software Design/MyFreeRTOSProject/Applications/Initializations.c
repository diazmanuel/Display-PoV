/**
 * @file Initializations.c
 * @brief Contiene la inicializacion de proyecto.
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

#include "Oscilador.h"
#include "UART.h"
#include "Bluetooth.h"
#include "Initializations.h"
#include "myTimers.h"
#include "Shifter.h"
#include "InterruptHandler.h"
#include "StatusHandler.h"
#include "Display.h"

/**
	\fn  void Inicializar ( void )
	\brief Inicializacion de Hardware.
 	\param void
	\return:void
*/
void Inicializar ( void )
{
	InitPLL ( ) ;
	//Bluetooth_Init(115200);//tarda alrededor de 4 segundos en ejecutarse completamente
							 //debido a limitaciones del HC05 que hay que tener
							//en cuenta a la hora de RESETEARLO Y CONFIGURARLO

	InitStatusFlags();
	InitShifter();
	InitHallSensor();
	InitTimer0();
	InitTimer1();
	UART_Init(115200);
	TestPointInitGeneral();
	InitSystick();


}

void InitSystick(void)
{

	STRELOAD = ( STCALIB) - 1;	// Recarga cada 10 ms a 100MHz
	STCURR = 0;	// Cargando con cero limpio y provoco el disparo de una intrrupcion
	STCTRL |= ((1<<ENB)|(1<<MYINT)|(1<<CLK));
}

