/*
 * port.h
 *
 * Created: 10.12.2015 20:16:07
 *  Author: Admin
 */ 


#ifndef PORT_H_
#define PORT_H_

#if defined (__AVR_ATmega328P__)
#  include "atxOS_m328p.h"
#elif defined (__AVR_ATxmega128A1__)
#  include "atxOS_x128a1.h"
#endif

#endif /* PORT_H_ */