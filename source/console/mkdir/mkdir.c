/*
 * mkdir.c
 * Creates a directory in the current working directory.
 * Created: 24.02.2016 17:57:19
 *  Author: Admin
 */ 
#include "../console.h"
#include "../../atxOS/fs/utfs/utfs_core.h"

void mkdir(char* args) {
	if (*args) {
		mkDir(sfts(currentDir()->typesector), args);
	} else {
		writeString("No directory name provided. \r\n");
	}
}
