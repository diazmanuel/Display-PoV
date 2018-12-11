/**
 * @file SD.c
 * @brief Funcionalidades de la tarjeta SD.
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
#include "SD.h"
#include "ff.h"
#include "integer.h"
static FATFS fs;           /**< FatFs work area needed for each volume */
static FIL fp;             /**< File object needed for each open file */

/**
 * @fn void SD_Read()
 * @brief Carga la mariz de la imagen desde un archivo.txt
 * @details El archivo debe contener en texto plano la informacion de la matriz imagen
 * lista para ser leida y copiada al buffer en RAM.
 */
void SD_Read(){

	char buffer;
	int i,n=0;
	UINT j;

	if (f_mount(&fs, "", 0) != FR_OK)
	{

	}


    if (f_open(&fp, FILENAME, FA_READ | FA_OPEN_ALWAYS) == FR_OK) {
    	       for(i=0;i<filas*columnas;i++){
    	        	f_read(&fp,&buffer,1,&j);
    	        	Image.Buffers[0][n]=buffer;
    	        	Image.Buffers[1][n]=buffer;
    	        	n++;
    	        }
    	        f_close(&fp);
    }
}
