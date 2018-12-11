/**
 * @file StatusHandler.h
 * @brief Asociaciones a StatusHandler.c
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
#ifndef STATUSHANDLER_H_
#define STATUSHANDLER_H_

#define ON	1
#define OFF	0

extern unsigned int Status_Flags;
	/**
	 * @def REFRESHTIMER_ALREADY_SETUP
	 * Si este bit esta en 1, indica que el timer1 ya fue configurado. Marca 0 de lo contrario.
	 */
	#define		REFRESHTIMER_ALREADY_SETUP	0
	/**
	 * @def STREAM
	 * Bit de status que controla la comunicacion de la uart.
	 * Si esta en 0 indica que se esta procesando un dato, en 1 indica que estoy esperando un dato.
	 */
	#define 	STREAM						1
	/**
	 * @def REFRESHDATA
	 * Bit de status que indica si la ilera de 32 leds debe ser actualizada.
	 */
	#define 	REFRESHDATA					2
	/**
	 * @def BT_CONNECTED
	 * Bit de status que indica si se estableció comunicacion por bluetooth con el HC05.
	 */
	#define 	BT_CONNECTED				3
	/**
	 * @def PHASE_RESET
	 * Bit de status que indica cuando debe haber correccion de fase en la impresion de la imagen.
	 * Si esta en 1, hay que corregir.
	 */
	#define 	PHASE_RESET					4


void InitStatusFlags (void);

#endif /* STATUSHANDLER_H_ */
