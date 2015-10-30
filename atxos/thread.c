/*
 * thread.c
 *
 * Created: 29.10.2015 19:28:04
 *  Author: Admin
 */ 
#include "thread.h"
#include "hardware.h"

extern THREAD threads[MAX_THREADS];

BYTE* initStack(BYTE* stack, void* entrypoint) {
	BYTE i;

	//push for debug, if a thread exits this will be its return address by now.
	*stack = (BYTE) 0x66;
	stack--;
	*stack = (BYTE) 0x66;
	stack--;
	*stack = (BYTE) 0x66;
	stack--;
	*stack = (BYTE) 0x66;
	stack--;
	//TODO:proper exit address
	//entrypoint
	*stack = LOBYTE(entrypoint);
	stack--;
	*stack = HIBYTE(entrypoint);
	stack--;
	*stack = (BYTE) (0x00);
	stack--;
	//create initial stack
	*stack = (BYTE) 0x00; //R0
	stack--;
	*stack = (BYTE) 0x80; //flags int enabled
	stack--;
	*stack = (BYTE) 0x00; //R1
	stack--;
	//R2 to R31, R24 and R25 can be used to provide arguments
	for (i = 0x02; i <= 0x31; i++) {
		*stack = (BYTE) i;
		stack--;
		if ((i & (BYTE) 0x0f) == (BYTE) 0x09) {
			i += (BYTE) 0x06;
		}
	}
	
	return stack;
}


char CreateThread(void* entrypoint) {
	BYTE n;
	for (n = 1; n < MAX_THREADS; n++) {
		if (!threads[n].pStack) {
			goto freeslot;
		}
	}
	return -1;
	
	freeslot:
	threads[n].pStack = initStack((void*)(0x4000 - n * 512), entrypoint);
	return 0;
}
