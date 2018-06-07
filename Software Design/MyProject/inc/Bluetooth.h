#include "RegsLPC1769.h"


#define ResetHC05 1,30
#define ATHC05 1,31
#define NAME "AT+NAME=TD2-G8\r\n"
#define PASSWORD "AT+PSWD=1234\r\n"
#define ROL "AT+ROLE=0\r\n"


void IntToAscii(uint32_t BaudRate, uint8_t *buffer,uint32_t lenght,uint32_t n);
void MandarAT(uint8_t* buff);
void ResetearHC05();
void ConfigPinBluetooth();
void Bluetooth_Init(uint32_t);


