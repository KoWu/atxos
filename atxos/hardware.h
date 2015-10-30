/*
 * hardware.h
 *
 * Created: 30.10.2015 10:43:07
 *  Author: Admin
 */ 


#ifndef HARDWARE_H_
#define HARDWARE_H_

#define FREQ 100000
#define MAX_THREADS 8

//datatypes
#define BYTE unsigned char
#define WORD unsigned int
#define DWORD unsigned long

#define LOBYTE(x) ((BYTE)((WORD)x))
#define HIBYTE(x) ((BYTE)(((WORD)x) >> 8))

#endif /* HARDWARE_H_ */