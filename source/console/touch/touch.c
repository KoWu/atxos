/*
 * touch.c
 * Creates a file in the current working directory.
 * Created: 24.02.2016 19:48:41
 *  Author: Admin
 */ 
#include "../console.h"

void touch(char* args) {
	if (*args) {
		mkFil(0, sfts(currentDir()->typesector), args);
		} else {
		writeString("No filename provided. \r\n");
	}
}