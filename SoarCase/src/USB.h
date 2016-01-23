/*
 * USB.h
 *
 * Created: 23.04.2015 21:33:10
 *  Author: InfangerIvo
 */


#ifndef USB_H_
#define USB_H_


#include "Descriptors.h"
#include "Globals.h"
#include "Input.h"


//void SetupHardware ( void );
void EVENT_USB_Device_Connect ( void );
void EVENT_USB_Device_Disconnect ( void );
void EVENT_USB_Device_ConfigurationChanged ( void );
void EVENT_USB_Device_ControlRequest ( void );
void EVENT_USB_Device_StartOfFrame ( void );
bool CALLBACK_HID_Device_CreateHIDReport ( USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
        uint8_t* const ReportID,
        const uint8_t ReportType,
        void* ReportData,
        uint16_t* const ReportSize );
void CALLBACK_HID_Device_ProcessHIDReport ( USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
        const uint8_t ReportID,
        const uint8_t ReportType,
        const void* ReportData,
        const uint16_t ReportSize );


#endif /* USB_H_ */