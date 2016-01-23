/*
 * WDT.h
 *
 * Created: 08.10.2015 22:08:15
 *  Author: InfangerIvo
 */ 


#ifndef WDT_H_
#define WDT_H_

#include <avr/io.h>

#define WDT_Reset()	__asm__ __volatile__("wdr") //( watchdog_reset( ) )


void WDT_EnableAndSetTimeout( WDT_PER_t period );
void WDT_Disable( void );


#endif /* WDT_H_ */