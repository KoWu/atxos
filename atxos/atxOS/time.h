/*
 * time.h
 *
 * Created: 12.12.2015 17:57:13
 *  Author: Admin
 */ 


#ifndef TIME_H_
#define TIME_H_

#include "hardware.h"


DWORD CurrentTime();

void Sleep(WORD s);

void Msleep(WORD ms);

#endif /* TIME_H_ */