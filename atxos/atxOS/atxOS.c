/*
 * atxOS.c
 *
 * Created: 05.11.2015 16:54:46
 *  Author: Admin
 */ 
#include "atxOS.h"
#include "internals.h"
#include "scheduler.h"
#include "dispatcher.h"
#include "thread.h"
#include "port/port.h"

volatile TCB* currentThread = 0;

void atxOSInit() {
	portSetupTimer();
	
	//copy existing stack
	register BYTE* from = SP;
	register BYTE* to = (BYTE*)(SP - sizeof(TCB));
	register uint16_t size = ATXOS_PORT_RAMSTART + ATXOS_PORT_RAMSIZE - SP;
	for (register uint16_t n = size; n; n--) {
		*to++ = *from++;
	}
	SP = (WORD)(to - size);
	
	//fill tcb
	TCB* tcb = ATXOS_PORT_RAMSTART + ATXOS_PORT_RAMSIZE - sizeof(TCB);
	tcb->nextThread = tcb;
	tcb->nextWaiting = 0;
	tcb->TopOfStack = (void*)0xaffe;
	tcb->status = T_STATUS_ACTIVE;
	tcb->prioLevel = SCHED_MIN_PRIO;
	tcb->prioCurrent = 0;
	currentThread = tcb;
}

void __attribute__((naked)) atxOSStart() {
	asm volatile (\
	"sei \n\t"\
	"rjmp idle\n\t");
}

void idle() {
	for (;;) {
		ContextSwitch();
	}
}
