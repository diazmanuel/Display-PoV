/**
 * @file Display.c
 * @brief Funciones asociadas a la entidad del proyecto.
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
#include "Display.h"
#include "RegsLPC1769.h"
#include "myTimers.h"
#include "GPIOLPC1769.h"
#include "PinoutConfigLPC1769.h"
#include "Shifter.h"
#include "HallSensor.h"
#include "InterruptHandler.h"
#include "StatusHandler.h"
#include "Serial.h"



/**
 * @fn void DisplaySend(Shifter_t *This_Shifter)
 * @brief Inicia la sencuencia de impresion
 * @details Se abstrajo de PrintStart para sumar una capa de nivel al programa.
 * @param This_Shifter  Puntero a la estructura
 */
void DisplaySend(Shifter_t *This_Shifter)
{
	PrintStart(This_Shifter);
}

/**
 * @fn void DisplayStop(void)
 * @brief Detiene la sencuencia de impresion
 * @details Se abstrajo de PrintStop para sumar una capa de nivel al programa.
 */
void DisplayStop(void)
{
	PrintStop();
}
/**
 * @fn void PrintStart(Shifter_t *This_Shifter)
 * @brief Indica el inicio de la secuencia de impresion.
 * @details Reinicia el indice de PWM, arranca el timer 0 encargado del shifteo y coloca los
 * bits menos significativos (que es de donde parte la secuencia) en los pines fisicos de hardware.
 * @param This_Shifter Puntero a la estructura
 */
void PrintStart(Shifter_t *This_Shifter)
{
	PwmStateIndex=0;
	SetPIN(D0_PIN,(This_Shifter->Data[0][PwmStateIndex])&1);	//Pongo el LSB en el pin, y arranco la secuencia.
	SetPIN(D1_PIN,(This_Shifter->Data[1][PwmStateIndex])&1);
	SetPIN(D2_PIN,(This_Shifter->Data[2][PwmStateIndex])&1);
	SetPIN(D3_PIN,(This_Shifter->Data[3][PwmStateIndex])&1);
	T0TCR &=~ (1<<MYCR);//Arranco timer 0 (Shifter clock)
	T0TCR |= (1<<CE);
}
/**
 * @fn void PrintStop(void)
 * @brief Indica el final de la secuencia de impresion
 * @details Para y reinicia el timer 0 y el indice de shifteo de la informacion.
 */
void PrintStop(void)
{
	T0TCR &=~(1<<CE);	//Paro timer 0 (shifter clock)
	T0TCR |= (1<<MYCR);
	ShiftCounter=0;
}

/**
 * @fn void CargaData(Shifter_t *This_Shifter)
 * @brief Funcion de traduccion de buffers de datos.
 * @details Aplica una transformacion a los datos recibidos descomprimidos de la aplicacion de PC
 * en un tipo de dato unsigned int y aplica logica de ordenamiento para dejar los datos
 * listos para ser interpretados por el hardware en un Shifter_t.
 * @param This_Shifter Puntero a donde alojar el dato post-procesado.
 */
void CargaData(Shifter_t *This_Shifter)
{
	static uint8_t IndexFila=0;
	uint8_t N_Color,N_FilaShifter,N_Led,N_PWM;
	uint8_t Intensidad, LedInfo;
	for(N_FilaShifter=0;N_FilaShifter<FILAS_SHIFTERS;N_FilaShifter++){ 	//Primero recorro que fila de shifter es
		for(N_Led=0;N_Led<(N_LEDS/FILAS_SHIFTERS);N_Led++){				//Luego recorro todos los Leds de c/fila
			LedInfo=Image.Buffers[MYIR][(IndexFila*N_LEDS)+(N_FilaShifter*(N_LEDS/FILAS_SHIFTERS))+N_Led];
			for(N_Color=0;N_Color<3;N_Color++){							//Luego recorro todos los colores de c/Led (R, G y B)
				/*
				 * Por como esta ordenado el Buffer descomprimido: (R->G->B)
				 * Es decir cuando 	N_Color=0 => Informacion de Rojo
				 * 					N_Color=1 => Informacion de Verde
				 * 					N_Color=2 => Informacion de Azul.
				 */
				Intensidad = (LedInfo>>N_Color*2)&0x03; //Evaluo la intensidad de cada color para cargar el PWM. Toma valores de 0-3
				for(N_PWM=0;N_PWM<Intensidad;N_PWM++){	//Por ultimo recorro cada estado de PWM cargando la cantidad de intensidad necesaria.
					/*
					 * Por como esta ordenado el Shifter_t->Data: (G->B->R)
					 * Cuando		N_Color=0 => Informacion del Verde
					 * 				N_Color=1 => Informacion del Azul
					 * 				N_Color=2 => Informacion del Rojo
					 */

					This_Shifter->DataSource->Data[N_FilaShifter][N_PWM] |= (1<<(((N_Color+2)%3)+N_Led*3));

					/*
					 * (N_Color+2)%3 realiza la transformacion necesaria para el ordenamiento de colores de un buffer a otro.
					 */
				}
			}
		}
	}

		IndexFila++;
		IndexFila%=N_CIR;
		if(PhaseRefresh==1){
			IndexFila=0;
			PhaseRefresh=0;
		}
}

/*
 * Image.Buffers[2][4096] (Peina para adentro, arranca en la ultima fila dessde la derecha)
 * La trama quedo armada de la siguiente manera:
 * LED					0	1	2	3	4	5	6	7	8	9	10			...					31
 * Shifters Asociados	|------FilaShifter 0--------|	|----Shifter 1 --	...	--Fila_Shifter 3-|
 * Leds de fila			0	1	2	3	4	5	6	7	0	1	2			...					7
 *
 * Si quisieras leer el dato de una fila de 32 leds de corrido. Tendrias que ordenar los datos de la siguiente forma
 *
 * 	Shifter->Data[3] Shifter->Data[2] Shifter->Data[1] Shifter->Data[0]
 * 	Si lo leyeras en este orden, estarias leyendo una fila de led desde el centro hacia afuera.
 */

/**
 * @fn void Descomprimir (void)
 * @brief Funcion de descompresion
 * @details Toma el dato post-procesado por el algoritmo lz4 y lo
 * guarda en el buffer de la matriz imagen completa.
 * Por ultimo levanta el status flag indicando que esta listo para recibir y procesar otro dato.
 */
void Descomprimir (void)
{
	   LZ4_decompress_safe_usingDict(
	               (const char*)Image.Stream_Buffer + 10,
	               (char*)Image.Buffers[IW],
	               Image.Stream_Size - 10,
	               N_PIX,
	               (const char*)Image.Buffers[MYIR],
	               N_PIX
	           );
     Image.Buffer_Index = (Image.Buffer_Index + 1) % 2;
     Status_Flags|=(ON<<STREAM);

}

/**
 * @fn void Stream(void)
 * @brief Aviso de refresco a la APP.
 * @details Avisa a la aplicacion de pc, una vez recibido el dato, que puede enviar uno nuevo.
 */
void Stream(void){
	uint8_t A=1;
	if (( Status_Flags>>STREAM)&&ON){
		 Serial_PushTx(A); //para stream
	}
}


/**
 * @fn void TestPointInitGeneral(void)
 * @brief Inicializa Test Points en Hardware
 *
 * Declaro todos los test points genericos del hardware
 * y les grabo una secuencia para medir.
 */
void TestPointInitGeneral(void)
{
	SetPINSEL(TPGPIO1,0);
	SetDIR(TPGPIO1,1);	//Salida
	SetPIN(TPGPIO1,0);

	SetPINSEL(TPGPIO2,0);
	SetDIR(TPGPIO2,1);
	SetPIN(TPGPIO2,0);

	SetPINSEL(TPGPIO3,0);
	SetDIR(TPGPIO3,1);
	SetPIN(TPGPIO3,0);

	SetPINSEL(TPGPIO4,0);
	SetDIR(TPGPIO4,1);
	SetPIN(TPGPIO4,0);

	SetPINSEL(TPGPIO5,0);
	SetDIR(TPGPIO5,1);
	SetPIN(TPGPIO5,0);
}

