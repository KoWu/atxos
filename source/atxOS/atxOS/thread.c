/*
 * Thread.c
 * Responsible for spawning threads.
 * Created: 05.11.2015 14:51:29
 *  Author: Admin
 */ 

#include "atxOS.h"
#include "thread.h"
#include "hardware.h"
#include "internals.h"
#include <util/atomic.h>
#include "port/port.h"

/************************************************************************/
/* Initializes the stack for a new thread.                              */
/************************************************************************/
TCB* initStack(void* start, BYTE* stack, int sizeOfStack) {
	TCB* tcb;
	BYTE i;
	
	stack += (sizeOfStack - sizeof(TCB));
	tcb = (TCB*) stack--;
	
	//push for debug, if a thread exits this will be its return address by now.
	//TODO: proper exit address
	*stack-- = (BYTE) 0x66;
	*stack-- = (BYTE) 0x66;
	*stack-- = (BYTE) 0x66;
	*stack-- = (BYTE) 0x66;

	//push entrypoint
	*stack-- = LOBYTE(start);
	*stack-- = HIBYTE(start);
	#if (ATXOS_PORT_RETADDRSIZE == 3)
	*stack-- = (BYTE) (0x00);
	#endif

	//create initial stack
	*stack-- = (BYTE) 0x00; //R0
	*stack-- = (BYTE) 0x80; //global interrupts enabled flag set
	*stack-- = (BYTE) 0x00; //R1
	
	//fill R2 to R31 with debug content, R24 and R25 can be used to provide arguments
	//TODO: possibility to provide arguments
	for (i = 0x02; i <= 0x31; i++) {
		*stack-- = (BYTE) i;
		if ((i & (BYTE) 0x0f) == (BYTE) 0x09) {
			i += (BYTE) 0x06;
		}
	}
	
	//set top of stack
	tcb->TopOfStack = stack;
	return tcb;
}

/************************************************************************/
/* Creates a new thread.                                                */
/************************************************************************/
ATXVALUE CreateThread(void *start, BYTE* stack, int stackSize, ThreadPriority priority){
	TCB* tcb;
	tcb = initStack(start, stack, stackSize);
	
	tcb->status = T_STATUS_ACTIVE;
	tcb->prioLevel = priority;
	tcb->nextWaiting = 0;
	tcb->prioCurrent = 0;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		tcb->nextThread = currentThread->nextThread;
		currentThread->nextThread = tcb;
	}
	
	return ATX_OK;
}
