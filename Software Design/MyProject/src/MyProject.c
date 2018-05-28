/*
===============================================================================
 Name        : MyProject.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include "lz4.h"
#include "Initializations.h"
#include "Serial.h"
#include "Flags.h"



struct Buffer Image={0};
static uint8_t IndexFila=0;


void CargaData(){
	uint8_t N_Color,N_FilaShifter,N_LED,N_PWM;
	for(N_FilaShifter=0;N_FilaShifter<4;N_FilaShifter++){
		for(N_PWM=0;N_PWM<3;N_PWM++){
			Image.Data[N_FilaShifter][N_PWM]=0;
			for(N_LED=0;N_LED<8;N_LED++){
				for(N_Color=0;N_Color<3;N_Color++){
					Image.Data[N_FilaShifter][N_PWM]|=(((Image.Buffers[IR][(IndexFila*32)+(N_FilaShifter*8)+N_LED]<<(N_Color*2) )&0x03)<=N_PWM ? 0x00 : 0x01<<((N_LED*3)+N_Color) );
				}
			}
		}
	}


	//23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
	// B  G  R  B  G  R  B  G  R  B  G  R  B  G  R  B  G  R  B  G  R  B  G  R

	WriteFlag(DataRefresh,0);
	IndexFila++;
	if(IndexFila==128){
		IndexFila=0;
	}
}


int main(void) {
Init();

    while(1) {
//    	if(ReadFlag(InterruptHall)){
//    		WriteFlag(InterruptHall,0);
//    		IndexFila=0;
    	}
    	if(ReadFlag(DataRefresh)){
    		CargaData();
    	}
        if(ReadFlag(Decompress)){
     	   LZ4_decompress_safe_usingDict(
     	               (const char*)Image.Stream_Buffer + 10,
     	               (char*)Image.Buffers[IW],
     	               Image.Stream_Size - 10,
     	               N_PIX,
     	               (const char*)Image.Buffers[IR],
     	               N_PIX
     	           );
            Image.Buffer_Index = (Image.Buffer_Index + 1) % 2;
     	   WriteFlag(Decompress,0);
     	   //Serial_PushTx('1'); //para stream
        }else{
     	   SerialManager ();
        }
return 0 ;
}
