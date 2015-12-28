/*
 * Thread.c
 *
 * Created: 05.11.2015 14:51:29
 *  Author: Admin
 */ 

#include "atxOS.h"
#include "thread.h"
#include "hardware.h"
#include "internals.h"
#include <util/atomic.h>
#include "port/port.h"


TCB* initStack(void* start, BYTE* stack, int sizeOfStack) {
	TCB* tcb;
	BYTE i;
	
	stack += (sizeOfStack - sizeof(TCB));
	tcb = (TCB*) stack--;
	
	//push for debug, if a thread exits this will be its return address by now.
	*stack-- = (BYTE) 0x66;
	*stack-- = (BYTE) 0x66;
	*stack-- = (BYTE) 0x66;
	*stack-- = (BYTE) 0x66;

	//TODO:proper exit address
	//entrypoint
	*stack-- = LOBYTE(start);
	*stack-- = HIBYTE(start);
	#if (ATXOS_PORT_RETADDRSIZE == 3)
	*stack-- = (BYTE) (0x00);
	#endif

	//create initial stack
	*stack-- = (BYTE) 0x00; //R0
	*stack-- = (BYTE) 0x80; //flags int enabled
	*stack-- = (BYTE) 0x00; //R1
	
	//R2 to R31, R24 and R25 can be used to provide arguments
	for (i = 0x02; i <= 0x31; i++) {
		*stack-- = (BYTE) i;
		if ((i & (BYTE) 0x0f) == (BYTE) 0x09) {
			i += (BYTE) 0x06;
		}
	}
	
	tcb->status = T_STATUS_ACTIVE;
	tcb->nextWaiting = 0;
	tcb->prioLevel = 0;
	tcb->TopOfStack = stack;
	
	return tcb;
}

int CreateThread(void *start, BYTE* stack, int stackSize){
	TCB* newTCB;
	newTCB = initStack(start, stack, stackSize);
	
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		newTCB->nextThread = currentThread->nextThread;
		currentThread->nextThread = newTCB;
	}
	
	return 0;
}
