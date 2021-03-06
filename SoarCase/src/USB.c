/*
 * USB.c
 *
 * Created: 23.04.2015 21:34:45
 *  Author: InfangerIvo
 */

#include "USB.h"


/** Buffer to hold the previously generated Keyboard HID report, for comparison purposes inside the HID class driver. */
static uint8_t PrevKeyboardHIDReportBuffer[sizeof ( USB_KeyboardReport_Data_t )];


/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_HID_Device_t Keyboard_HID_Interface = {
    .Config =
    {
        .InterfaceNumber              = INTERFACE_ID_Keyboard,
        .ReportINEndpoint             =
        {
            .Address              = KEYBOARD_EPADDR,
            .Size                 = KEYBOARD_EPSIZE,
            .Banks                = 1,
        },
        .PrevReportINBuffer           = PrevKeyboardHIDReportBuffer,
        .PrevReportINBufferSize       = sizeof ( PrevKeyboardHIDReportBuffer ),
    },
};


/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect ( void ) {
    //LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}


/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect ( void ) {
    //LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}


/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged ( void ) {
    bool ConfigSuccess = true;

    ConfigSuccess &= HID_Device_ConfigureEndpoints ( &Keyboard_HID_Interface );

    USB_Device_EnableSOFEvents();

    //LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}


/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest ( void ) {
    HID_Device_ProcessControlRequest ( &Keyboard_HID_Interface );
}


/** Event handler for the USB device Start Of Frame event. */
void EVENT_USB_Device_StartOfFrame ( void ) {
    HID_Device_MillisecondElapsed ( &Keyboard_HID_Interface );
}


/** HID class driver callback function for the creation of HID reports to the host.
 *
 *  \param[in]     HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in,out] ReportID    Report ID requested by the host if non-zero, otherwise callback should set to the generated report ID
 *  \param[in]     ReportType  Type of the report to create, either HID_REPORT_ITEM_In or HID_REPORT_ITEM_Feature
 *  \param[out]    ReportData  Pointer to a buffer where the created report should be stored
 *  \param[out]    ReportSize  Number of bytes written in the report (or zero if no report is to be sent)
 *
 *  \return Boolean \c true to force the sending of the report, \c false to let the library determine if it needs to be sent
 */
bool CALLBACK_HID_Device_CreateHIDReport ( USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
        uint8_t* const ReportID,
        const uint8_t ReportType,
        void* ReportData,
        uint16_t* const ReportSize ) {

    USB_KeyboardReport_Data_t* KeyboardReport = ( USB_KeyboardReport_Data_t* ) ReportData;

    uint8_t i;
    uint8_t UsedKeyCodes = 0;

    for ( i = 0; i < 20; i++ ) {
        if ( Input_IsButtonPressedHID ( i ) == TRUE && Input_getHIDCode ( i ) != 0 && Input_HasHIDPriority ( i ) == TRUE )
            KeyboardReport->KeyCode[UsedKeyCodes++] = Input_getHIDCode ( i );
    }

    //if ( UsedKeyCodes )
    //    KeyboardReport->Modifier = HID_KEYBOARD_MODIFIER_LEFTSHIFT;

    Input_ClearStateHID();

    *ReportSize = sizeof ( USB_KeyboardReport_Data_t );
    return false;
}


/** HID class driver callback function for the processing of HID reports from the host.
 *
 *  \param[in] HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in] ReportID    Report ID of the received report from the host
 *  \param[in] ReportType  The type of report that the host has sent, either HID_REPORT_ITEM_Out or HID_REPORT_ITEM_Feature
 *  \param[in] ReportData  Pointer to a buffer where the received report has been stored
 *  \param[in] ReportSize  Size in bytes of the received HID report
 */
void CALLBACK_HID_Device_ProcessHIDReport ( USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
        const uint8_t ReportID,
        const uint8_t ReportType,
        const void* ReportData,
        const uint16_t ReportSize ) {
    /*
    uint8_t  LEDMask   = LEDS_NO_LEDS;
    uint8_t* LEDReport = (uint8_t*)ReportData;

    if (*LEDReport & HID_KEYBOARD_LED_NUMLOCK)
      LEDMask |= LEDS_LED1;

    if (*LEDReport & HID_KEYBOARD_LED_CAPSLOCK)
      LEDMask |= LEDS_LED3;

    if (*LEDReport & HID_KEYBOARD_LED_SCROLLLOCK)
      LEDMask |= LEDS_LED4;

    LEDs_SetAllLEDs(LEDMask);
    */
}