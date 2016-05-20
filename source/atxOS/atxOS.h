/*
 * atxOS.h
 * Main include file.
 * Created: 05.11.2015 17:04:30
 *  Author: Admin
 */ 


#ifndef ATXOS_H_
#define ATXOS_H_

#include "thread.h"
#include "semaphore.h"
#include "mutex.h"
#include "time.h"

#include "hardware.h"

/************************************************************************/
/* Initializes the OS. System functions can be called afterwards.       */
/************************************************************************/
void AtxOSInit();

/************************************************************************/
/* Starts the OS. There is no return from here.                         */
/************************************************************************/
void AtxOSStart();

#endif /* ATXOS_H_ */