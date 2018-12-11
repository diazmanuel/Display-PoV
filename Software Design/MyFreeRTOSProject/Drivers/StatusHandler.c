/**
 * @file StatusHandler.c
 * @brief Variable de Status
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
#include "StatusHandler.h"

unsigned int Status_Flags; /**< Variable de status de programa*/

/**
 * @fn void InitStatusFlags (void)
 * @brief Inicializa el status de programa.
 */
void InitStatusFlags (void)
{
	Status_Flags=0x00000000;
}
