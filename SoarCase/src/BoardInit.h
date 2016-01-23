#ifndef BOARDINIT_H_
#define BOARDINIT_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Globals.h"
#include "TWI.h"

#ifdef LUFA
	#include <LUFA/Platform/XMEGA/ClockManagement.h>
#endif

/*! CPU speed 32MHz, BAUDRATE 100kHz and Baudrate Register Settings */
//#define CPU_SPEED   32000000
#define BAUDRATE	800000
#define TWI_BAUDSETTING TWI_BAUD(F_CPU, BAUDRATE)


typedef struct GPIO_struct {
    PORT_t * GPIO_Port;
    register8_t GPIO_Pin;
} GPIO_t;


enum USB_State {
    USB_NotConnected = 0,
    USB_HIDConnected = 1,
};


GPIO_t LED_Green;
GPIO_t LED_Red;
GPIO_t LED_Blue;
GPIO_t BT_Connected;
GPIO_t S1;
GPIO_t S2;
GPIO_t S3;
GPIO_t S4;
GPIO_t S5;
GPIO_t S6;
GPIO_t S7S;
GPIO_t S7AI;
GPIO_t S7BI;
GPIO_t S7AO;
GPIO_t S7BO;
GPIO_t S8S;
GPIO_t S8AI;
GPIO_t S8BI;
GPIO_t S8AO;
GPIO_t S8BO;


TWI_Master_t twiMaster;    /*!< TWI master module. */

void BoardInit ( void );
void Int_SysClkInit ( void );
void Ext_SysClkInit ( void );
void LUFA_SysClkInit();
void GPIOInit ( void );


void TWIInit ( void );
void ActivateISR ( void );
void SysTickInit ( void );
void CounterInit ( void );


void SetOutput ( GPIO_t output );
void ClearOutput ( GPIO_t output );
void ToggleOutput ( GPIO_t output );
uint8_t ReadInput ( GPIO_t input );


#endif /* BOARDINIT_H_ */