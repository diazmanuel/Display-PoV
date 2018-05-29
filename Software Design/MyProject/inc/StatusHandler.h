/*
 * StatusHandler.h
 *
 *  Created on: May 21, 2018
 *      Author: germi
 */

#ifndef STATUSHANDLER_H_
#define STATUSHANDLER_H_

#define ON	1
#define OFF	0

extern unsigned int Status_Flags;
	#define		REFRESHTIMER_ALREADY_SETUP	0

void InitStatusFlags (void);

#endif /* STATUSHANDLER_H_ */
