/*
 * console.h
 *
 * Created: 20.02.2016 15:15:11
 *  Author: Admin
 */ 


#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "../atxOS/hardware.h"
#include "../atxOS/thread.h"
#include "../atxOS/driver/char/uart.h"

typedef struct stringfunc {
	char* string;
	void (*func)(void);
} stringfunc;

void register_console();

#endif /* CONSOLE_H_ */