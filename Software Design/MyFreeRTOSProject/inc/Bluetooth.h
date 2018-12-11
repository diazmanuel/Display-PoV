/**
 * @file Bluetooth.h
 * @brief Asociaciones a Bluetooth.c
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
#include "RegsLPC1769.h"


#define ResetHC05 1,4
#define ATHC05 1,0
#define STATE_PIN	1,1


#define NAME "AT+NAME=TD2-G8\r\n" ///@def Ver datasheet del modulo bluetooth HC05
#define PASSWORD "AT+PSWD=1235\r\n"///@def Ver datasheet del modulo bluetooth HC05
#define ROL "AT+ROLE=0\r\n"///@def Ver datasheet del modulo bluetooth HC05


void IntToAscii(uint32_t BaudRate, uint8_t *buffer,uint32_t lenght,uint32_t n);
void MandarAT(uint8_t* buff);
void ResetearHC05();
void ConfigPinBluetooth();
void Bluetooth_Init(uint32_t);
void TestPointInitBT(void);


