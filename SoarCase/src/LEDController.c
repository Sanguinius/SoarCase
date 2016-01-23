/*
 * LEDController.c
 *
 * Created: 24.11.2014 20:52:26
 *  Author: InfangerIvo
 */
#include "LEDController.h"

uint16_t LEDOff =		0b0000000000000000;
uint16_t LEDOn =		0b1111111111111111;
uint16_t LEDBlink8x =	0b0101010101010101;
uint16_t LEDBlink2x =	0b0000111100001111;
uint16_t LEDBlink1x =	0b0000000011111111;
uint16_t LEDPauseShort = 0b0011111111111111;
uint16_t LEDFlash1x =	0b1000000000000000;
uint16_t LEDFlash2x =	0b1010000000000000;
uint16_t LEDFlash3x =	0b1010100000000000;
uint16_t LEDFlash4x =	0b1010101000000000;
uint16_t LEDFlash5x =	0b1010101010000000;
uint16_t LEDFlash6x =	0b1010101010100000;
uint16_t LEDFlash8x =   0b1010101010101010;

uint16_t counter = 0;
uint8_t serialErrorTime = 0;


void LEDController_Init()
{

}


void LEDController_UpdateCounter()
{
    //Count 16x and then reset -> every 1 second
    if ( counter++ >= 15 )
        counter = 0;
}


extern void LEDController_UpdatePattern ( void )
{
    if ( Serial_State == Serial_Error_BufferOverFlow || serialErrorTime > 0){
        LEDController_Blink ( LED_Red, LEDFlash2x );
		if (serialErrorTime == 0)
			serialErrorTime = 40;
		serialErrorTime--;
		}
        
    if ( USB_State == USB_NotConnected )
        LEDController_Blink ( LED_Blue, LEDFlash1x );
        
    else
        LEDController_Blink ( LED_Blue, LEDPauseShort );
        
    //LEDController_Blink ( LED_Blue, LEDFlash8x );
    //LEDController_Blink ( LED_Red, LEDBlink2x );
    //LEDController_Blink ( LED_Green, LEDBlink8x );
}


void LEDController_Blink ( GPIO_t led, uint16_t pattern )
{
    if ( pattern & ( 1 << counter ) )
        SetOutput ( led );
        
    else
        ClearOutput ( led );
}