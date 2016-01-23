/*
 * Serial.h
 *
 * Created: 26.10.2014 15:34:39
 *  Author: InfangerIvo
 */
#ifndef SERIAL_H_
#define SERIAL_H_

#include "Globals.h"
#include "BoardInit.h"
#include "nmea/nmea.h"
#include "Buzzer.h"


#define BUFFERLENGTH 255
#define BUFFERTEMPLENGTH 127
#define BUFFERDIRIN 0
#define BUFFERDIROUT 1


extern struct Baudrate_struct {
    uint32_t Baud9600;
    uint32_t Baud19200;
    uint32_t Baud38400;
    uint32_t Baud57600;
    uint32_t Baud115200;
} Baudrate;


enum BufferSelect {
    USBCDC = 0,
    PDA = 1,
    GPS = 2,
    BT = 3,
    ExtTTL = 4,
    ExtRS = 5,
    Intern = 6,
    BufferCount = 7,
    BufferAll = 8,
    AllSerial = 9,
};


enum Serial_ErrorCodes {
    Serial_Error_NoError = 0,
    Serial_Error_InitalisationError = 1,
    Serial_Error_BufferOverFlow = 2,
};

enum NMEA_Code {
    NMEA_GPGGA = 1, //
    NMEA_GPGSA = 2, //
    NMEA_GPGSV = 3, //
    NMEA_GPRMC = 4, //
    NMEA_GPVTG = 5, //
    NMEA_PGRMZ = 10, //
    NMEA_PMTK = 11, //
    NMEA_PSCSO = 12, //
    NMEA_PSCSW = 13, //
    AT_OK = 20, //
    NMEA_UNKNOWNNMEA = 98, //
    NMEA_NONMEA = 99, //
};


typedef struct BufferPerDirection {
    uint8_t Buffer[BUFFERLENGTH];
    uint8_t bufferSize;
    uint16_t writePos;
    uint16_t readPos;
    uint8_t isFull;
    uint8_t isEmpty;
    uint8_t dataSizeInBuffer;
    uint8_t packetCountInBuffer;
    uint8_t dataToSend;
    uint8_t locked;
    uint8_t isReading;
};

typedef struct BufferPerSerial {
    struct BufferPerDirection BufferIn;
    struct BufferPerDirection BufferOut;
    bool isActivated;
    uint32_t baudSetting;
    USART_t *port;
};

struct Buffer_struct {
    struct BufferPerSerial UART[BufferCount];
} Buffer;

nmeaINFO info;
nmeaPARSER parser;

void Serial_Init ( void );
void Serial_BufferInit ( void );

void NMEAParser_init ( void );

//void Serial_changeBaudrate ( uint8_t port, uint32_t baudrate );
void Serial_changeBaudrate ( uint8_t target, uint32_t baudrate );

void sendChar ( uint8_t target );
void receiveChar ( uint8_t target );

void SerialInitPDA ( uint32_t baudrate );
void SerialStartPDA ( void );
void SerialEndPDA ( void );
void SerialInitExtTTL ( uint32_t baudrate );
void SerialStartExtTTL ( void );
void SerialEndExtTTL ( void );
void SerialInitExtRS ( uint32_t baudrate );
void SerialStartExtRS ( void );
void SerialEndExtRS ( void );
void SerialInitBT ( uint32_t baudrate );
void SerialStartBT ( void );
void SerialEndBT ( void );
void SerialInitGPS ( uint32_t baudrate );
void SerialStartGPS ( void );
void SerialEndGPS ( void );

void SerialDataFrom ( uint8_t source );

void Buffer_Clear ( uint8_t dir, uint8_t target );
void Buffer_CharIn ( char c, uint8_t dir, uint8_t target );
char Buffer_CharOut ( uint8_t dir, uint8_t target );
char Buffer_CheckChar ( uint8_t dir, uint8_t target, uint8_t position );
uint8_t Buffer_CheckNMEA ( uint8_t dir, uint8_t target );

void Buffer_StringIn ( char *string, uint8_t length, uint8_t dir, uint8_t target );
uint8_t Buffer_StringOut ( char *string, uint8_t dir, uint8_t target );
uint8_t Buffer_DataOut ( char *string, uint8_t length, uint8_t dir, uint8_t target );

void Buffer_WritePosUp ( uint8_t dir, uint8_t target );
void Buffer_ReadPosUp ( uint8_t dir, uint8_t target );
void Buffer_CheckState ( uint8_t dir, uint8_t target );

void Buffer_MoveData ( uint8_t source, uint8_t target );

void SerialManager ();

uint8_t SetupGPS ( uint32_t baudrate, uint8_t refreshrate );
uint8_t SetupBT ( void );


#endif /* SERIAL_H_ */