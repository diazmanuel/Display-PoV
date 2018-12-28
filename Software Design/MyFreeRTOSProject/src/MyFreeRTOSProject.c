/*! \mainpage Display POV Project
 *
 * \section intro_sec Introduccion
 *
 * El firmware fue diseñado en base a un sistema operativo en tiempo real, cuyo propósito final es mostrar una imagen mediante el efecto de la persistencia de la vision humana.
 *
 * \section install_sec Secciones de funcionamiento
 *
 *  \subsection step1 Secuencia de inicio:
 *
 * Al iniciar el programa, se concentrará específicamente en calibrar su frecuencia de refresco de imagen para poder trabajar adecuadamente (independientemente de poseer una imagen a imprimir). Esto se logra gracias a la señal del sensor de efecto hall, de la cual se toman sucesivas muestras que, luego de un simple filtro FIR, indicarán el periodo de trabajo del motor, es decir, el tiempo que tarda en dar una vuelta.
 * A partir de este momento, el sistema se encuentra listo y se habilita para realizar el resto de sus tareas habituales.
 *
 *  \subsection step2 Obtención de la imagen en formato fuente:
 *
 * El sistema ahora estará a la espera de una conexión por bluetooth de la cual adquiera los datos necesarios para almacenar una imagen. Mientras no tenga éxito en la conexión, tomará su imagen fuente de una tarjeta micro SD por SPI. De tener éxito, el sistema se encargará de establecer comunicación, solicitar y validar la información requerida, esta última será enviada de forma comprimida  para reducir el tamaño del paquete de información.
 *
 * \subsection step3 Procesado y almacenamiento de la imagen en formato final:
 *
 * Una vez recibido el dato, será momento de descomprimirlo, interpretarlo, traducirlo y almacenarlo para posteriormente poder imprimirlo. Cabe destacar que la información enviada por la aplicación de PC y la información requerida por el firmware son de tipos y tamaños completamente diferentes, por los que necesitan una transformación especial.
 * Por lo tanto, la información primero sufre descompresión por el algoritmo lz4, luego transformación de tipo y por último se almacena en memoria estática esperando ser solicitada.
 *
 * \section lib_sec Librerias Asociadas
 *
 * Para este proyecto, ademas del codigo fuente que encontrara en
 *
 * https://github.com/CastroGerman/TD2-Project
 *
 * se deberá linkear las siguientes librerias de LPCOpen proporcionadas por NXP:
 *
 * -	lpc_board_nxp_lpcxpresso_1769
 * -	lpc_chip_175x_6x
 *
 * Estas librerias no se encuentran dentro del repositorio ya que son de libre acceso.
 *
 */

/**
 * @file MyFreeRTOSProject.c
 * @brief Main del software
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
#include "InterruptHandler.h"
#include "Shifter.h"
#include "HallSensor.h"
#include "Initializations.h"
#include "Display.h"
#include "SD.h"
#include "Serial.h"
#include "board.h"
/**
 * @fn int main (void)
 * @brief Funcion principal
 */
int main (void){
	CrearShifter(&Shifter);
	CrearHallSensor(&HallSensor);
	Inicializar();
	DisplayStop();
	DisplaySend(Shifter);

	/*=========SPI=======*/
	Board_SSP_Init(LPC_SSP1);
    Chip_SSP_Init(LPC_SSP1);
    Chip_SSP_Enable(LPC_SSP1);

	SD_Read();

	int hola=0;
	while(1)
	{

		/*if(hola==50000)
		{
			HallSensor->TiempoVuelta=T1TC;
			T1TC=0x00000000;
			Interrupt_Flags|=(1<<HALLSENSOR_INT);
			hola=0;
		}
		hola++;
*/

		CheckForInterrupt(Shifter,HallSensor);
	}
    return 0 ;
}

