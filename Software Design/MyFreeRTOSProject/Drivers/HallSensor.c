/**
 * @file HallSensor.c
 * @brief Funcionalidades del sensor hall.
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
#include "HallSensor.h"
#include "RegsLPC1769.h"
#include "PinoutConfigLPC1769.h"
#include "GPIOLPC1769.h"
#include "InterruptHandler.h"
#include "StatusHandler.h"
#include "stdlib.h"

int PhaseRefresh; /**< Indicador de que hay que corregir la fase*/

/**
 * @var	HallSensor
 * @brief Global Pointer to a HallSensor_t
 *
 * El puntero principal que maneja la info del sensor hall.
 */
HallSensor_t *HallSensor;
/**
 * @fn void InitHallSensor(void)
 * @brief Inicializa el hardware asociado al sensor hall.
 * @return void
 */
void InitHallSensor(void)
{
	SetPINSEL(HALL_PIN, 1); 			// Pin:P2[11] 	GPIO / EINT1
	MYEXTMODE |= ( 1 << EXTMODE1 );		// externa por flanco
	MYEXTPOLAR |= ( 1 << EXTPOLAR1 ); 	// ascendente
	ISER0 |= ( 0x01 <<ISE_EINT1 ) ;		// Habilito Interrupcion externa 1
}
/**
 * @fn void CrearHallSensor(HallSensor_t **HallSensorHEAD)
 * @brief Crea y aloja en RAM estatica la memoria necesaria.
 * @param HallSensorHEAD: Direccion del puntero.
 * @return void
 */
void CrearHallSensor(HallSensor_t **HallSensorHEAD)
{
	HallSensor_t *Main;
	Main = malloc(sizeof(HallSensor_t));
	*HallSensorHEAD = Main;
}
/**
 * @fn void EliminarHallSensor(HallSensor_t **HallSensorHEAD)
 * @brief Elimina la memoria previamente alojada.
 * @param HallSensorHEAD: Direccion del puntero.
 * @return void
 */
void EliminarHallSensor(HallSensor_t **HallSensorHEAD)
{
	free(*HallSensorHEAD);
}

/**
 * @fn void EINT1_IRQHandler(void)
 * @brief Handler de interrupcion del NVIC del Cortex-M3.
 *
 * Asociada a EINT1, disparada por el sensor de efecto hall, encargada de registrar y reiniciar
 * la cuenta del timer 1 y dar aviso al sistema operativo.
 * @return void
 */
void EINT1_IRQHandler(void)
{
	HallSensor->TiempoVuelta=T1TC;
	T1TC=0x00000000;	//Reinicio cuenta (Puedo hacerlo tambien por CR en T1TCR)
	MYEXTINT |= ( 1 << EINT1 );	// borro el flag EINT1 de interrupcion externa 1 del registro EXTINT
	PhaseRefresh=1;
	Interrupt_Flags|=(ON<<HALLSENSOR_INT);
}
/**
 * @fn void HallSensorInterrupt(HallSensor_t *This_HallSensor)
 * @brief Sensa y da aviso de la velocidad del motor.
 * @details Sensa multiples veces el periodo que tarda el motor en dar una vuelta, lo procesa mediante un
 * simple filtro FIR y cuando alcance un valor estable dentro de un acotrado margen de error
 * (que lo cubrira de posibles vibraciones, ventiscas y/o cualquier factor que pueda alterar su periodo de giro)
 * indicara al sistema operativo que ese es el valor verdadero de periodo para setear un timer (timer1).
 * A partir de este momento en adelante, dicho timer será el dictador de la frecuencia que deberan
 * tener los leds para refrescar su imagen.
 * @param This_HallSensor: Direccion de memoria de informacion util.
 * @return void
 */
void HallSensorInterrupt(HallSensor_t *This_HallSensor)
{
	int CotaSuperior=(This_HallSensor->TiempoVuelta)*(1.001);	//El 0.1% de la cuenta
	int CotaInferior=(This_HallSensor->TiempoVuelta)*(0.999);	//Si se realiza un define de esto, debe estar debidamente casteado para que no exista redondeo.
	if((CotaInferior<(This_HallSensor->WorkingFrequency))&&((This_HallSensor->WorkingFrequency)<CotaSuperior)) //Admito margen del 0.1%
	{
		if(!((Status_Flags>>REFRESHTIMER_ALREADY_SETUP)&ON))	//Si NO ESTA setteado, lo setteo.
		{
			//Configuro el timer encargado del refresh.
			Interrupt_Flags|=(ON<<REFRESHTIMER_SETUP);
		}
	}else{
		//Vamos promediando para tener un valor mas exacto. Filtro FIR de 2 muestras.
		This_HallSensor->WorkingFrequency= (This_HallSensor->WorkingFrequency + This_HallSensor->TiempoVuelta)/2;
	}
}
