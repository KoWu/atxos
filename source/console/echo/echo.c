/*
 * echo.c
 * Prints the provided args onto the uart.
 * Created: 24.02.2016 17:40:13
 *  Author: Admin
 */ 
#include "../console.h"

void echo(char* args) {
	writeString(args);
}