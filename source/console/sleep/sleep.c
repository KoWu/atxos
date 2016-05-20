/*
 * sleep.c
 * Sleeps for n seconds.
 * Created: 24.02.2016 18:40:33
 *  Author: Admin
 */
#include "../console.h"

void sleep(char* args) {
	Sleep(atoi(args));
}