/*
 * LEDController.h
 *
 * Created: 24.11.2014 20:52:43
 *  Author: InfangerIvo
 */
#ifndef LEDCONTROLLER_H_
#define LEDCONTROLLER_H_

#include <avr/io.h>
#include "Globals.h"
#include "BoardInit.h"
#include "Serial.h"
#include "BoardInit.h"


extern void LEDController_Init ( void );
extern void LEDController_UpdateCounter ( void );
extern void LEDController_UpdatePattern ( void );
void LEDController_Blink ( GPIO_t led, uint16_t pattern );



#endif /* LEDCONTROLLER_H_ */