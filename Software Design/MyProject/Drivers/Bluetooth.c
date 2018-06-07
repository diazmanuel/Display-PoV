#include "GPIOLPC1769.h"
#include "Serial.h"
#include "Bluetooth.h"
#include "PinoutConfigLPC1769.h"


void IntToAscii(uint32_t BaudRate, uint8_t *buffer,uint32_t lenght,uint32_t n){
	uint32_t i,j,aux;
	for(i=0;i<lenght;i++){//vacio buffer
		buffer[i]='\0';
	}

	for(j=0;j<n;j++){
		aux=1;
		for(i=0;i<n-1-j;i++){
		aux=aux*10;
		}

		buffer[j]=48+ ((int)(BaudRate/aux)); //el 0 en ascii es el 48

		BaudRate-=((int)(BaudRate/aux))*aux; //voy restando el baudrate digito por digito
	}

}

void MandarAT(uint8_t* buff){
	uint32_t i=0;
	while(buff[i]!= '\0'){
		Serial_PushTx(buff[i]);
		i++;
	}
	for(i=0;i<5000000;i++);
}

void ResetearHC05() {
	uint32_t i;
	SetPIN(ResetHC05,0); //Fuerzo reset con flanco ascendente en el pin( 0 y luego 1)
	for(i=0;i<100000;i++);

	SetPIN(ResetHC05,1);
	for(i=0;i<8000000;i++);
}

void ConfigPinBluetooth(){
SetPINSEL(ResetHC05,0); //Pin de Reset de Bluetooth
SetPINSEL(ATHC05,0); //Pin para entrar en Comandos AT de Bluetooth

SetDIR(ATHC05,1);//salida
SetDIR(ResetHC05,1);//salida
}

void Bluetooth_Init(uint32_t BaudRate){

	uint8_t ConfigBaudInicio[]="AT+UART=";
	uint8_t ConfigBaudFinal[]=",0,0\r\n";//1 bit de stop, sin paridad
//	uint8_t preguntar[]="AT+UART?\r\n";
//	uint8_t at[]="AT\r\n";
	uint8_t nombre[]=NAME;
	uint8_t clave[]=PASSWORD;
	uint8_t rol[]=ROL; //0 esclavo - 1 maestro
	uint8_t buffer[10];//para convertir el BaudRate en un "string" de chars
	uint8_t n=1;
	uint32_t aux=9;

	while(aux<BaudRate){
		aux= aux*10 +9;
		n++;
	}

	IntToAscii(BaudRate,buffer,10,n);
	UART_Init(38400);
	ConfigPinBluetooth();
	SetPIN(ATHC05,1);
	ResetearHC05();
	SetPIN(ATHC05,0);

	//para saber que manda por at, ver el nombre del parametro
	// e ir a las variables declaradas

//	MandarAT(at);
	MandarAT(ConfigBaudInicio);
	MandarAT(buffer);//buffer contiene el baudrate convertido en un vector de chars
	MandarAT(ConfigBaudFinal);
//	MandarAT(preguntar);
	MandarAT(clave);
	MandarAT(rol);
	MandarAT(nombre);

	ResetearHC05();

}


