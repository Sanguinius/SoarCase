/*
 * WDT.c
 *
 * Created: 07.10.2015 22:53:46
 *  Author: InfangerIvo
 */ 

#include "WDT.h"
#include <avr/io.h>


void WDT_EnableAndSetTimeout( WDT_PER_t period )
{
	uint8_t temp = WDT_ENABLE_bm | WDT_CEN_bm | period;
	
	CCP = CCP_IOREG_gc; //Disable protected I/O registers
	WDT.CTRL = temp;
	
	while (WDT.STATUS==WDT_SYNCBUSY_bm){
	}
}



void WDT_Disable( void )
{
	uint8_t temp = (WDT.CTRL & ~WDT_ENABLE_bm) | WDT_CEN_bm;
	CCP = CCP_IOREG_gc;
	WDT.CTRL = temp;
}