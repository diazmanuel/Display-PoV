/**
 * @file myTasks.c
 * @brief Todas las tareas del S.O.
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
#include "myTasks.h"
#include "Shifter.h"
#include "HallSensor.h"
#include "Display.h"
#include "Initializations.h"
#include "InterruptHandler.h"
#include "StatusHandler.h"
#include "myTimers.h"
#include "GPIOLPC1769.h"
#include "SD.h"

xSemaphoreHandle	stick,					/**< Semaforo binario para tPeriodic*/
					sHallSensor,			/**< Semaforo binario para tHallSensor*/
					sRefreshTimerSetup,		/**< Semaforo binario para tRefreshTimerSetup*/
					sCargaData,				/**< Semaforo binario para tCargaData*/
					sDescomprimir;			/**< Semaforo binario para tDescomprimir*/



void disk_timerproc();



/*
 * CrearQueues(void) Crea queues que seran usadas como mailboxes.
 * A mailbox is used to hold data that can be read by any task, or any interrupt service
 * routine. The data does not pass through the mailbox, but instead remains in the
 * mailbox until it is overwritten. The sender overwrites the value in the mailbox. The
 * receiver reads the value from the mailbox, but does not remove the value from the
 * mailbox.
 */
/*
void CrearQueues(void)
{
	pqShifter = xQueueCreate(1, sizeof(Shifter_t *));
	pqHallSensor = xQueueCreate(1, sizeof(HallSensor_t *));
}
xQueuePeek(pqShifter, &Shifter, (portTickType) 0);
xQueueOverwrite(pqShifter,(void *) &Shifter);
*/

/**
 * @fn void CrearSemaforos(void)
 * @brief Crea Semaforos necesarios para las tareas del S.O.
 */
void CrearSemaforos(void)
{
	vSemaphoreCreateBinary(stick);
	vSemaphoreCreateBinary(sHallSensor);
	vSemaphoreCreateBinary(sRefreshTimerSetup);
	vSemaphoreCreateBinary(sCargaData);
	vSemaphoreCreateBinary(sDescomprimir);

	xSemaphoreTake(stick,(portTickType) 1);
	xSemaphoreTake(sHallSensor,(portTickType) 1);
	xSemaphoreTake(sRefreshTimerSetup,(portTickType) 1);
	xSemaphoreTake(sCargaData,(portTickType) 1);
	xSemaphoreTake(sDescomprimir,(portTickType) 1);
}

/**
 * @fn void CrearTareas(void)
 * @brief Crea las tareas requeridas por el S.O.
 */
void CrearTareas(void)
{
	xTaskCreate(tPeriodic,(signed char *) "tPeriodic",configMINIMAL_STACK_SIZE,NULL,						(tskIDLE_PRIORITY + 1UL),(xTaskHandle *) NULL);
	xTaskCreate(tInit,(signed char *) "tInit",configMINIMAL_STACK_SIZE,NULL,								(tskIDLE_PRIORITY + 4UL),(xTaskHandle *) NULL);
	xTaskCreate(tHallSensor,(signed char *) "tHallSensor",configMINIMAL_STACK_SIZE,NULL,					(tskIDLE_PRIORITY + 3UL),(xTaskHandle *) NULL);
	xTaskCreate(tRefreshTimerSetup,(signed char *) "tRefreshTimerSetup",configMINIMAL_STACK_SIZE,NULL,		(tskIDLE_PRIORITY + 3UL),(xTaskHandle *) NULL);
	xTaskCreate(tCargaData,(signed char *) "tCargaData",configMINIMAL_STACK_SIZE*2,NULL,					(tskIDLE_PRIORITY + 3UL),(xTaskHandle *) NULL);
	xTaskCreate(tDescomprimir,(signed char *) "tDescomprimir",configMINIMAL_STACK_SIZE*4,NULL,				(tskIDLE_PRIORITY + 2UL),(xTaskHandle *) NULL);
}

/**
 * @fn void vApplicationTickHook(void)
 * @brief 10ms tickhook
 * @warning this is called by an interrupt handler!
 */
void vApplicationTickHook(void)
{
	portBASE_TYPE xTaskSwitchRequired = pdFALSE;
	xSemaphoreGiveFromISR(stick, &xTaskSwitchRequired);
	portEND_SWITCHING_ISR(xTaskSwitchRequired);
}
/**
 * @fn void vApplicationIdleHook(void)
 * @brief Idle Hook from FreeRTOS
 * @details La tarea de menor prioridad del sistema.
 * Se encarga de recibir los datos de la aplicacion de PC por bluetooth.
 * Una vez terminado de recibir el paquete comprimido, da semaforo a tDescomprimir.
 */
void vApplicationIdleHook(void)
{
	while(1)
	{
		SerialManager();
    }
}

/**
 * @fn void tPeriodic (void *pv)
 * @brief Marca cada cuanto pido informacion a la aplicacion de PC
 * @param pv Pointer to void
 */

void tPeriodic (void *pv)
{
	xSemaphoreTake(stick, (portTickType) 0);
	disk_timerproc();
	// Para el SPI/FatFS, se debe llamar c/ 10ms, cuando cierro el archivo deja de molestar
	int i=0;
	while(1)
	{
		static uint8_t DiscountSteam=0;
		if (DiscountSteam==0){
			Stream();
			DiscountSteam=REFRESH_STREAM_S;
		}
		DiscountSteam--;

	//*************SIMULAR HALL********************
/*
		  i++;

			if(i==1)
			{

				HallSensor->TiempoVuelta=T1TC;
				T1TC=0x00000000;
			    Status_Flags|=(ON<<PHASE_RESET);
			    Status_Flags&=~(0x01<<PHASE_RESET);

			    aa=1;
				xSemaphoreGive(sHallSensor);
				i=0;
			}
*/
		//********************************************

		xSemaphoreTake(stick, portMAX_DELAY);
	}
}
/**
 * @fn void tInit(void *pv)
 * @brief Tarea de inicializacion de software.
 * @details Se ejecuta una unica vez.
 * @param pv
 */
void tInit(void *pv)
{
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

	vTaskDelete(NULL);
}
/**
 * @fn void tHallSensor(void *pv)
 * @brief Task asociada a la interrupcion del sensor hall.
 * @details Encargada de hacer el sensado de la velocidad del motor.
 * @param pv
 */
void tHallSensor(void *pv)
{
	xSemaphoreTake(sHallSensor, (portTickType) 0);
	while(1)
	{
		HallSensorInterrupt(HallSensor);

		if((Status_Flags>>REFRESHTIMER_ALREADY_SETUP)&ON)	//Si ESTA setteado, elimino el task.
		{
			vTaskDelete(NULL);
		}
		xSemaphoreTake(sHallSensor, portMAX_DELAY);
	}
}
/**
 * @fn void tRefreshTimerSetup(void *pv)
 * @brief Configura Timer 1.
 * @details Se ejecuta una unica vez.
 * @param pv Pointer to void
 */
void tRefreshTimerSetup(void *pv)
{
	xSemaphoreTake(sRefreshTimerSetup, portMAX_DELAY);
	while(1)
	{

		RefreshTimerSetup(HallSensor);
		vTaskDelete(NULL);
		xSemaphoreTake(sRefreshTimerSetup, portMAX_DELAY);
	}
}
/**
 * @fn void tCargaData(void *pv)
 * @brief Asociada a la carga del Shifter_t
 * @details Encargada de desglosar la siguiente linea de la imagen a ser impresa
 * y cargarla como dato futuro en una estructura Shifter_t
 * @param pv Pointer to void
 */
void tCargaData(void *pv)
{
	xSemaphoreTake(sCargaData, (portTickType) 0);
	while(1)
	{
		CargaData(Shifter);
		xSemaphoreTake(sCargaData, portMAX_DELAY);
	}
}
/**
 * @fn void tDescomprimir(void *pv)
 * @brief Asociada al algoritmo de descompresion.
 * @param pv Pointer to void
 */
void tDescomprimir(void *pv)
{
	xSemaphoreTake(sDescomprimir, (portTickType) 0);
	while(1)
	{
		Descomprimir();
		xSemaphoreTake(sDescomprimir, portMAX_DELAY);
	}
}

