/*
 **************************************************************************
 * @file    buttoncontrol.c
 * @author  Ivo Infanger
 * @version V01_01
 * @date    03.01.2012
 * @brief   Button Configuration & Setup
 **************************************************************************
 */

#include "Input.h"
#include "LUFA/Drivers/USB/USB.h"
#include "Serial.h"


#define PS1 0
#define PS2 1
#define PS3 2
#define PS4 3
#define PS5 4
#define PS6 5
#define PS7 6
#define PS8 7
#define PS7AO 8
#define PS7BO 9
#define PS7AI 10
#define PS7BI 11
#define PS8AO 12
#define PS8BO 13
#define PS8AI 14
#define PS8BI 15


struct Input_struct {
    GPIO_t GPIO;
    uint8_t GPIO_State;
    uint8_t State;
    uint32_t UpTimer;
    uint8_t State_Old;
    uint8_t State_Special;
};


struct Button_struct {
    uint8_t State_ASCII;
    uint8_t State_HID;
    char Code_ASCII;
    char Code_HID;
    bool HIDPriority;
    uint8_t Doubleclick;
};
struct Input_struct Input[16];
struct Button_struct Button[20];



uint8_t UpTimeDebounce = 5;


void Input_Init ( void )
{
    uint8_t i;
    
    //Assign Ports
    Input[PS1].GPIO = S1;
    Input[PS2].GPIO = S2;
    Input[PS3].GPIO = S3;
    Input[PS4].GPIO = S4;
    Input[PS5].GPIO = S5;
    Input[PS6].GPIO = S6;
    Input[PS7].GPIO = S7S;
    Input[PS8].GPIO = S8S;
    Input[PS7AO].GPIO = S7AO;
    Input[PS7BO].GPIO = S7BO;
    Input[PS7AI].GPIO = S7AI;
    Input[PS7BI].GPIO = S7BI;
    Input[PS8AO].GPIO = S8AO;
    Input[PS8BO].GPIO = S8BO;
    Input[PS8AI].GPIO = S8AI;
    Input[PS8BI].GPIO = S8BI;
    
    
    //Initialize reset
    for ( i = 0; i <= 15; i++ ) {
        Input[i].GPIO_State = 0;
        Input[i].UpTimer = 0;
        Input[i].State_Special = 0;
        
        //Set Input.State to the state of the input
        Input[i].State = ReadInput ( Input[i].GPIO );
        Input[i].State_Old = Input[i].State;
    }
    
    //Set assignements
    Input_AssignButtons();
    
    //Read inputs
    Input_ReadInputs();
}


void Input_AssignButtons()
{
    Button[FS1].Code_HID = HID_KEYBOARD_SC_ESCAPE;
    Button[FS1].Code_ASCII = 0;
    Button[FS1].HIDPriority = TRUE;
    Button[FS2].Code_HID = HID_KEYBOARD_SC_F6; //Alternates
    Button[FS2].Code_ASCII = 0;
    Button[FS2].HIDPriority = TRUE;
    Button[FS3].Code_HID = HID_KEYBOARD_SC_F5; //Waypoints
    Button[FS3].Code_ASCII = 0;
    Button[FS3].HIDPriority = TRUE;
    Button[FS4].Code_HID = HID_KEYBOARD_SC_F1; //QuickMenu
    Button[FS4].Code_ASCII = 0;
    Button[FS4].HIDPriority = TRUE;
    Button[FS5].Code_HID = HID_KEYBOARD_SC_F2; //Analysis
    Button[FS5].Code_ASCII = 0;
    Button[FS5].HIDPriority = TRUE;
    Button[FS6].Code_HID = HID_KEYBOARD_SC_F3; //Notepad
    Button[FS6].Code_ASCII = 0;
    Button[FS6].HIDPriority = TRUE;
    
    Button[FS7OCCW].Code_HID = HID_KEYBOARD_SC_LEFT_ARROW;
    Button[FS7OCCW].Code_ASCII = 0;
    Button[FS7OCCW].HIDPriority = TRUE;
    Button[FS7OCW].Code_HID =  HID_KEYBOARD_SC_RIGHT_ARROW;
    Button[FS7OCW].Code_ASCII = 0;
    Button[FS7OCW].HIDPriority = TRUE;
    Button[FS7ICCW].Code_HID = HID_KEYBOARD_SC_DOWN_ARROW;
    Button[FS7ICCW].Code_ASCII = 0;
    Button[FS7ICCW].HIDPriority = TRUE;
    Button[FS7ICW].Code_HID = HID_KEYBOARD_SC_UP_ARROW;
    Button[FS7ICW].Code_ASCII = 0;
    Button[FS7ICW].HIDPriority = TRUE;
    Button[FS7ICCWAlt].Code_HID = HID_KEYBOARD_SC_Y;
    Button[FS7ICCWAlt].Code_ASCII = 0;
    Button[FS7ICCWAlt].HIDPriority = FALSE;
    Button[FS7ICWAlt].Code_HID = HID_KEYBOARD_SC_X;
    Button[FS7ICWAlt].Code_ASCII = 0;
    Button[FS7ICWAlt].HIDPriority = FALSE;
    Button[FS7].Code_HID = HID_KEYBOARD_SC_ENTER;
    Button[FS7].Code_ASCII = 0;
    Button[FS7].HIDPriority = TRUE;
    
    Button[FS8OCCW].Code_HID = HID_KEYBOARD_SC_O;
    Button[FS8OCCW].Code_ASCII = 0;
    Button[FS8OCCW].HIDPriority = FALSE;
    Button[FS8OCW].Code_HID =  HID_KEYBOARD_SC_P;
    Button[FS8OCW].Code_ASCII = 0;
    Button[FS8OCW].HIDPriority = FALSE;
    Button[FS8ICCW].Code_HID = HID_KEYBOARD_SC_K;
    Button[FS8ICCW].Code_ASCII = 0;
    Button[FS8ICCW].HIDPriority = FALSE;
    Button[FS8ICW].Code_HID = HID_KEYBOARD_SC_L;
    Button[FS8ICW].Code_ASCII = 0;
    Button[FS8ICW].HIDPriority = FALSE;
    Button[FS8ICCWAlt].Code_HID = HID_KEYBOARD_SC_N;
    Button[FS8ICCWAlt].Code_ASCII = 0;
    Button[FS8ICCWAlt].HIDPriority = FALSE;
    Button[FS8ICWAlt].Code_HID = HID_KEYBOARD_SC_M;
    Button[FS8ICWAlt].Code_ASCII = 0;
    Button[FS8ICWAlt].HIDPriority = FALSE;
    Button[FS8].Code_HID = HID_KEYBOARD_SC_B;
    Button[FS8].Code_ASCII = 0;
    Button[FS8].HIDPriority = FALSE;
}


void Input_CheckButton ( uint8_t PSx, uint8_t FSx, uint8_t edgePos )
{
    //Check positive edge (for normal buttons without 2th function
    if ( edgePos == TRUE ) {
        if ( Input[PSx].State == 0 && Input[PSx].State_Old == 1 )
            Input[PSx].State_Old = 0;
            
        if ( Input[PSx].State == 1 && Input[PSx].State_Old == 0 ) {
            Button[FSx].State_ASCII = 1;
            Button[FSx].State_HID = 1;
            Button[FSx].Doubleclick++;
            Input[PSx].State_Old = 1;
        }
    }
    
    //Check negative edge (for buttons with 2th function)
    else {
        if ( Input[PSx].State == 1 && Input[PSx].State_Old == 0 )
            Input[PSx].State_Old = 1;
            
        if ( Input[PSx].State == 0 && Input[PSx].State_Old == 1 ) {
            //If we have special function, 2th function was active so we don't set the push button function
            if ( Input[PSx].State_Special != 1 ) {
                Button[FSx].State_ASCII = 1;
                Button[FSx].State_HID = 1;
                Input[PSx].State_Old = 0;
            }
            
            else {
                Input[PSx].State_Special = 0;
                Input[PSx].State_Old = 0;
            }
        }
    }
}


void Input_CheckRotary ( uint8_t PSx, uint8_t PSxAO, uint8_t PSxBO, uint8_t PSxAI, uint8_t PSxBI, uint8_t FSx, uint8_t FSxOCW, uint8_t FSxOCCW, uint8_t FSxICW, uint8_t FSxICCW, uint8_t FSxICWAlt, uint8_t FSxICCWAlt )
{
    //Rotary encoder push-button
    Input_CheckButton ( PSx, FSx, FALSE );
    
    //Rotary encoder inner shaft
    if ( ( Input[PSxAI].State != Input[PSxAI].State_Old ) && Input[PSx].State == 0 ) {
        if ( Input[PSxAI].State == Input[PSxBI].State ) {
            Button[FSxICCW].State_ASCII = 1;
            Button[FSxICCW].State_HID = 1;
            Input[PSxAI].State_Old = Input[PSxAI].State;
        }
        
        if ( Input[PSxAI].State != Input[PSxBI].State ) {
            Button[FSxICW].State_ASCII = 1;
            Button[FSxICW].State_HID = 1;
            Input[PSxAI].State_Old = Input[PSxAI].State;
        }
    }
    
    //Rotary encoder inner shaft 2nd function
    if ( ( Input[PSxAI].State != Input[PSxAI].State_Old ) && Input[PSx].State == 1 ) {
        if ( Input[PSxAI].State == Input[PSxBI].State ) {
            Button[FSxICCWAlt].State_ASCII = 1;
            Button[FSxICCWAlt].State_HID = 1;
            Input[PSxAI].State_Old = Input[PSxAI].State;
            Input[PSx].State_Special = 1;
            //because of alternate function, we have to clear the push button old state,
            //so that we have no signal
            //Input[PSx].State_Old = 0;
        }
        
        if ( Input[PSxAI].State != Input[PSxBI].State ) {
            Button[FSxICWAlt].State_ASCII = 1;
            Button[FSxICWAlt].State_HID = 1;
            Input[PSxAI].State_Old = Input[PSxAI].State;
            Input[PSx].State_Special = 1;
            //because of alternate function, we have to clear the push button old state,
            //so that we have no signal
            //Input[PSx].State_Old = 0;
        }
    }
    
    //Rotary encoder outer shaft
    if ( Input[PSxAO].State != Input[PSxAO].State_Old ) {
        if ( Input[PSxAO].State == Input[PSxBO].State ) {
            Button[FSxOCCW].State_ASCII = 1;
            Button[FSxOCCW].State_HID = 1;
            Input[PSxAO].State_Old = Input[PSxAO].State;
        }
        
        if ( Input[PSxAO].State != Input[PSxBO].State ) {
            Button[FSxOCW].State_ASCII = 1;
            Button[FSxOCW].State_HID = 1;
            Input[PSxAO].State_Old = Input[PSxAO].State;
        }
    }
}


void Input_ClearStateASCII ( void )
{
    uint8_t i;
    
    for ( i = 0; i < 20; i++ )
        Button[i].State_ASCII = 0;
}


void Input_ClearStateHID ( void )
{
    uint8_t i;
    
    for ( i = 0; i < 20; i++ )
        Button[i].State_HID = 0;
}


bool Input_HasHIDPriority ( uint8_t FSx )
{
    return Button[FSx].HIDPriority;
}


void Input_ReadInputs ( void )
{
    uint8_t i;
    
    //Debounce all 16 inputs
    for ( i = 0; i <= 15; i++ ) {
        //Input[i].GPIO_State = ReadInput ( S2 );
        Input[i].GPIO_State = ReadInput ( Input[i].GPIO );
        
        if ( Input[i].GPIO_State != Input[i].State ) {
            Input[i].UpTimer++;
            
            if ( Input[i].UpTimer >= UpTimeDebounce )
                Input[i].State = Input[i].GPIO_State;
        }
        
        else
            Input[i].UpTimer = 0;
    }
    
    //Check Buttons S1 - S6
    Input_CheckButton ( PS1, FS1, TRUE );
    Input_CheckButton ( PS2, FS2, TRUE );
    Input_CheckButton ( PS3, FS3, TRUE );
    Input_CheckButton ( PS4, FS4 , TRUE );
    Input_CheckButton ( PS5, FS5, TRUE );
    Input_CheckButton ( PS6, FS6 , TRUE );
    
    //Check rotary encoders S7, S8
    Input_CheckRotary ( PS7, PS7AO, PS7BO, PS7AI, PS7BI, FS7, FS7OCW, FS7OCCW, FS7ICW, FS7ICCW, FS7ICWAlt, FS7ICCWAlt );
    Input_CheckRotary ( PS8, PS8AO, PS8BO, PS8AI, PS8BI, FS8, FS8OCW, FS8OCCW, FS8ICW, FS8ICCW, FS8ICWAlt, FS8ICCWAlt );
}


uint8_t Input_IsButtonPressedASCII ( uint8_t button )
{
    if ( Button[button].State_ASCII == 1 )
        return 1;
        
    else
        return 0;
}


uint8_t Input_IsButtonPressedHID ( uint8_t button )
{
    if ( Button[button].State_HID == 1 )
        return 1;
        
    else
        return 0;
}


void Input_SendNMEA ( nmeaINFO *info )
{
    for ( int i = 0; i <= 19; i++ ) {
        if ( Input_IsButtonPressedASCII ( i ) == 1 && ! ( USB_DeviceState == DEVICE_STATE_Configured && Input_HasHIDPriority ( i ) == TRUE ) ) {
            nmea_sendButton ( i, info );
        }
    }
}


uint8_t Input_getHIDCode ( uint8_t FSx )
{
    return Button[FSx].Code_HID;
}


void Input_ClearDoubleclick ( void )
{
    uint8_t i;
    
    for ( i = 0; i < 20; i++ )
        Button[i].Doubleclick = 0;
}


void Input_CheckDoubleclick ( void )
{
    uint8_t i;
    
    for ( i = 0; i < 20; i++ ) {
        if ( Button[i].Doubleclick >= 2 ) {
            Button[i].Doubleclick = 0;
            
            if ( i == FS1 ) {
            
                //Change NMEA Input
                if ( Serial_Enabled_GPS == TRUE )
                    SerialDataFrom ( ExtTTL );
                    
                else if ( Serial_Enabled_ExtTTL == TRUE )
                    SerialDataFrom ( GPS );
            }
            
            if ( i == FS2 ) {}
            
            //Enabe / disable HID
            if ( HID_Active == TRUE )
                HID_Active = FALSE;
                
            else
                HID_Active = TRUE;
        }
    }
}
