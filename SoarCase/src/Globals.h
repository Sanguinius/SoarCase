#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <avr/io.h>


#define XPLAINED

#define OFF 0
#define ON 1

#define TRUE 1
#define FALSE 0

#define SUCCESS 1
#define ERROR 0

uint8_t HID_Active;

uint8_t NMEA_From_GPS;
uint8_t NMEA_From_ExtTTL;
uint8_t NMEA_From_BT;
uint8_t NMEA_From_ExtRS;

uint8_t Serial_Enabled_PDA;
uint8_t Serial_Enabled_ExtTTL;
uint8_t Serial_Enabled_ExtRS;
uint8_t Serial_Enabled_BT;
uint8_t Serial_Enabled_GPS;
uint8_t Serial_Enabled_BaroInt;

uint8_t SerialOut;

uint8_t Serial_State;
uint8_t BT_State;
uint8_t TWI_State;
uint8_t USB_State;

volatile uint16_t rtc_counter_0_33Hz;
volatile uint16_t rtc_counter_1Hz;
volatile uint16_t rtc_counter_2Hz;
volatile uint8_t rtc_counter_10Hz;
volatile uint8_t rtc_counter_16Hz;
volatile uint8_t rtc_counter_100Hz;

volatile uint8_t tempFreqcounter;
uint16_t tempuint16_1;

volatile char receivedByteBT;
volatile char receivedByteGPS;
volatile char receivedBytePDA;
volatile char receivedByteExtTTL;
volatile char receivedBYteExtRS;

volatile uint8_t calcBaro;

#endif /* GLOBALS_H_ */