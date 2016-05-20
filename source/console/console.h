/*
 * console.h
 * A simple console implementation.
 * Created: 20.02.2016 15:15:11
 *  Author: Admin
 */ 


#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "../atxOS/hardware.h"
#include "../atxOS/thread.h"
#include "../atxOS/driver/char/uart.h"
#include "../atxOS/fs/utfs/utfs.h"

/************************************************************************/
/* Assignment of strings to function pointers.                          */
/************************************************************************/
typedef struct stringfunc {
	char* string;
	void (*func)(char*);
} stringfunc;


/************************************************************************/
/* Registers the console (creates a thread).                            */
/************************************************************************/
void register_console();

/************************************************************************/
/* Prints a string over uart.                                           */
/************************************************************************/
void writeString(char *str);

/************************************************************************/
/* Returns the current working directory.                               */
/************************************************************************/
utfs_fd *currentDir();

#endif /* CONSOLE_H_ */