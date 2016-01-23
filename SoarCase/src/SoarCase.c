#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "stdio.h"
#include "Globals.h"
#include "BoardInit.h"
#include "WDT.h"
#include "Input.h"
#include "Serial.h"
#include "LEDController.h"
#include "TWI.h"
#include "MS5611.h"
#include "Buzzer.h"
#include "USB.h"
#include "nmea/nmea.h"

//#define USB_HID_ON;

extern USB_ClassInfo_HID_Device_t Keyboard_HID_Interface;

uint8_t Serial_Enabled_PDA = TRUE;
uint8_t Serial_Enabled_ExtTTL = FALSE;
uint8_t Serial_Enabled_ExtRS = TRUE;
uint8_t Serial_Enabled_BT = FALSE;
uint8_t Serial_Enabled_GPS = FALSE;
uint8_t Serial_Enabled_BaroInt = FALSE;

int main ( void )
{
    //temp
    //char tempchar = 0;
    //uint8_t tempcounter = 0;
	
    //Init NMEA Info
    nmeaINFO info;
    nmea_zero_INFO ( &info );
    //char nmea_buff[40];
    //int8_t nmea_size;
    
    //Init Failure state
    Serial_State = Serial_Error_NoError;
    BT_State = 0;
    TWI_State = 0;
    USB_State = USB_NotConnected;
    
    uint16_t maincounter = 0;
    //uint16_t freq = 2000;
    //uint16_t duty = 1;
    
    //Delay until GPS has started
    _delay_ms ( 100 );
    
    BoardInit();
    
    Serial_Init();
    Serial_BufferInit();
    
    NMEAParser_init();
    
    Input_Init();
    
    //Set Serial output (PDA / EXTTTL / EXTRS / BT)
    if ( ReadInput ( S1 ) == TRUE )
        SerialOut = ExtTTL;      
    else
        SerialOut = PDA;
        
#ifdef USB_HID_ON
    USB_Init();
#endif
    
    MS5611_Init ( MS5611_OSR4096 );
    
    //Set QNH (testing)
    MS5611_SetQNH ( 1013.25 );
    
    //Standard Input
    SerialDataFrom ( ExtRS );
    
    //Buzzer init
    Buzzer_Init();
    Buzzer.State = BUZZER_STATUS_VARIOMODE;
    Buzzer_PlayMelody ( melodyStartup );
    
    //Setup GPS
    SetupGPS ( Baudrate.Baud38400, 1 );
    //SetupBT();
	
	//Init Watchdog
	WDT_EnableAndSetTimeout(WDT_PER_8CLK_gc);


    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    while ( 1 ) {
        if ( maincounter++ > 500 ) {
            //ToggleOutput ( LED_Green );
            maincounter = 0;
        }
        
        //Update Blue LED (BT Connected)
        /*
        if ( ReadInput ( BT_Connected ) == TRUE )
              SetOutput ( LED_Blue );
        
           else
              ClearOutput ( LED_Blue );
        */
#ifdef USB_HID_ON
        
        if ( USB_DeviceState == DEVICE_STATE_Configured ) {
            //SetOutput ( LED_Blue );
            USB_State = USB_HIDConnected;
        }
        
        else {
            //ClearOutput ( LED_Blue );
            USB_State = USB_NotConnected;
        }
        
#endif
        
        //Read all buttons and rotary encoders
        Input_ReadInputs();
        
        //Check doubleclick
        Input_CheckDoubleclick();
        
        //Polling USB task
#ifdef USB_HID_ON
        HID_Device_USBTask ( &Keyboard_HID_Interface );
        USB_USBTask();
#endif
        
        //Send button state over nmea
        Input_SendNMEA ( &info );
        Input_ClearStateASCII();
        
        //Update LED
        LEDController_UpdatePattern();
        
        //Buzzer Update
        Buzzer_Update();
        
        //New Barodata, send NMEA string
        if ( MS5611.NewData == TRUE ) {
            //char tempstring[50];
            //sprintf ( tempstring, "%"PRId32", %"PRId32", %"PRId32"\r\n",  MS5611.data.temperature, MS5611.data.pressure_raw , MS5611.data.pressure_filtered );
            //sprintf ( tempstring, "%"PRId32", %"PRId32", %"PRId32"\r\n",  MS5611.data.temperature, MS5611.data.altitude , MS5611.data.vario );
            //sendStringExtTTL ( tempstring );
            info.baroheight = ( MS5611.data.altitude / 100 );
            nmea_sendPGRMZ ( &info );
            //Buzzer_VarioSound ( MS5611.data.vario );
            MS5611.NewData = FALSE;
        }
		
		//Watchdog reset
		WDT_Reset();
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///USART E1 Interrupts (EXT TTL)
///////////////////////////////////////////////////////////////////////////////////////////////////
ISR ( USARTE1_RXC_vect )
{
    receiveChar ( ExtTTL );
}
ISR ( USARTE1_DRE_vect )
{
    sendChar ( ExtTTL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///USART F0 Interrupts (EXT RS232)
///////////////////////////////////////////////////////////////////////////////////////////////////
ISR ( USARTF0_RXC_vect )
{
    receiveChar ( ExtRS );
}
ISR ( USARTF0_DRE_vect )
{
    sendChar ( ExtRS );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///USART D0 Interrupts (GPS)
///////////////////////////////////////////////////////////////////////////////////////////////////
ISR ( USARTD0_RXC_vect )
{
    receiveChar ( GPS );
}
ISR ( USARTD0_DRE_vect )
{
    sendChar ( GPS );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///USART C1 Interrupts (Bluetooth)
///////////////////////////////////////////////////////////////////////////////////////////////////
ISR ( USARTC1_RXC_vect )
{
    receiveChar ( BT );
}
ISR ( USARTC1_DRE_vect )
{
    sendChar ( BT );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///USART C0 Interrupts (PDA)
///////////////////////////////////////////////////////////////////////////////////////////////////
ISR ( USARTC0_RXC_vect )
{
    receiveChar ( PDA );
}
ISR ( USARTC0_DRE_vect )
{
    SetOutput ( LED_Green );
    sendChar ( PDA );
    ClearOutput ( LED_Green );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///TWI Interrupt
///////////////////////////////////////////////////////////////////////////////////////////////////
ISR ( TWIC_TWIM_vect )
{
    TWI_MasterInterruptHandler ( &twiMaster );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///TCCO Interrupt
///////////////////////////////////////////////////////////////////////////////////////////////////
ISR ( TCC0_OVF_vect )
{
    //3s
    if ( ++rtc_counter_0_33Hz >= 1200 ) {
        rtc_counter_0_33Hz = 0;
        //Reset States
        Serial_State = Serial_Error_NoError;
		//Buzzer_PlayMelody(melodyTask);
    }
    
    //1s
    if ( ++rtc_counter_1Hz >= 400 ) {
        rtc_counter_1Hz = 0;
        Input_ClearDoubleclick();
        //MS5611_GetData();
    }
    
    //0.5s
    if ( ++rtc_counter_2Hz >= 200 ) {
        rtc_counter_2Hz = 0;
        
        if ( Serial_Enabled_BaroInt == TRUE )
            MS5611_GetData();
    }
    
    //100ms (10x 1s)
    if ( ++rtc_counter_10Hz >= 40 ) {
        rtc_counter_10Hz = 0;
        //MS5611_GetData();
    }
    
    //62.5ms (16x 1s)
    if ( ++rtc_counter_16Hz >= 25 ) {
        rtc_counter_16Hz = 0;
        LEDController_UpdateCounter();
        //MS5611_GetData();
    }
    
    //10ms (100x 1s)
    if ( ++rtc_counter_100Hz >= 4 ) {
        rtc_counter_100Hz = 0;
        //MS5611_GetData();
        SerialManager();
    }
}

