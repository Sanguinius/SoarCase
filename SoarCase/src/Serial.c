/*
 * Serial.c
 *
 * Created: 26.10.2014 15:34:14
 *  Author: InfangerIvo
 */

#include "Serial.h"

extern struct Baudrate_struct Baudrate = {9600, 19200, 38400, 57600, 115200};


///////////////////////////////////////////////////////////////////////////////////////////////////
///Serial_Init
///////////////////////////////////////////////////////////////////////////////////////////////////
void Serial_Init ( void )
{
    Buffer.UART[PDA].baudSetting = Baudrate.Baud57600;
    Buffer.UART[ExtTTL].baudSetting = Baudrate.Baud19200;
    Buffer.UART[ExtRS].baudSetting = Baudrate.Baud9600;
    Buffer.UART[BT].baudSetting = Baudrate.Baud57600;
    Buffer.UART[GPS].baudSetting = Baudrate.Baud9600;
    Buffer.UART[PDA].port = &USARTC0;
    Buffer.UART[ExtTTL].port = &USARTE1;
    Buffer.UART[ExtRS].port = &USARTF0;
    Buffer.UART[BT].port = &USARTC1;
    Buffer.UART[GPS].port = &USARTD0;
    
    SerialInitPDA ( Buffer.UART[PDA].baudSetting );
    SerialInitExtTTL ( Buffer.UART[ExtTTL].baudSetting );
    SerialInitExtRS ( Buffer.UART[ExtRS].baudSetting );
    SerialInitBT ( Buffer.UART[BT].baudSetting );
    SerialInitGPS ( Buffer.UART[GPS].baudSetting );
    
    if ( Serial_Enabled_PDA == TRUE )
        SerialStartPDA();
        
    if ( Serial_Enabled_ExtTTL == TRUE )
        SerialStartExtTTL();
        
    if ( Serial_Enabled_ExtRS == TRUE )
        SerialStartExtRS();
        
    if ( Serial_Enabled_BT == TRUE )
        SerialStartBT();
        
    if ( Serial_Enabled_GPS == TRUE )
        SerialStartGPS();
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///Serial_BufferInit
///////////////////////////////////////////////////////////////////////////////////////////////////
void Serial_BufferInit ( void )
{
    for ( int i = 0; i < BufferCount; i++ ) {
        Buffer.UART[i].isActivated = TRUE;
        Buffer.UART[i].BufferIn.bufferSize = BUFFERLENGTH;
        Buffer.UART[i].BufferOut.bufferSize = BUFFERLENGTH;
        Buffer.UART[i].BufferIn.isEmpty = TRUE;
        Buffer.UART[i].BufferOut.isEmpty = TRUE;
        Buffer.UART[i].BufferIn.isFull = FALSE;
        Buffer.UART[i].BufferOut.isFull = FALSE;
        Buffer.UART[i].BufferIn.writePos = 0;
        Buffer.UART[i].BufferOut.writePos = 0;
        Buffer.UART[i].BufferIn.readPos = 0;
        Buffer.UART[i].BufferOut.readPos = 0;
        Buffer.UART[i].BufferIn.dataSizeInBuffer = 0;
        Buffer.UART[i].BufferOut.dataSizeInBuffer = 0;
        Buffer.UART[i].BufferIn.packetCountInBuffer = 0;
        Buffer.UART[i].BufferOut.packetCountInBuffer = 0;
        Buffer.UART[i].BufferIn.dataToSend = 0;
        Buffer.UART[i].BufferOut.dataToSend = 0;
        Buffer.UART[i].BufferIn.locked = FALSE;
        Buffer.UART[i].BufferOut.locked = TRUE;
    }
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///NMEAParser_init
///////////////////////////////////////////////////////////////////////////////////////////////////
void NMEAParser_init ( void )
{
    nmeaINFO info;
    nmeaPARSER parser;
    nmea_zero_INFO ( &info );
    nmea_parser_init ( &parser );
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///Serial_changeBaudrate
///////////////////////////////////////////////////////////////////////////////////////////////////
void Serial_changeBaudrate ( uint8_t target, uint32_t baudrate )
{
    Buffer.UART[target].baudSetting = baudrate;
    
    Buffer.UART[target].port->BAUDCTRLA = ( ( ( ( F_CPU / baudrate ) + 8 ) / 16 ) - 1 ) & 255;
    Buffer.UART[target].port->BAUDCTRLB = ( ( ( ( F_CPU / baudrate ) + 8 ) / 16 ) - 1 ) >> 8;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///sendChar
///////////////////////////////////////////////////////////////////////////////////////////////////
void sendChar ( uint8_t target )
{
    if ( Buffer.UART[target].BufferOut.isEmpty == FALSE )
        Buffer.UART[target].port->DATA = Buffer_CharOut ( BUFFERDIROUT, target );
        
    else {
        Buffer.UART[target].port->CTRLA  &= ~ ( USART_DREINTLVL_LO_gc );
    }
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///receiveChar
///////////////////////////////////////////////////////////////////////////////////////////////////
void receiveChar ( uint8_t target )
{
    char receivedByte;
    receivedByte = Buffer.UART[target].port->DATA;
    Buffer_CharIn ( receivedByte, BUFFERDIRIN, target );
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///SerialInit
///////////////////////////////////////////////////////////////////////////////////////////////////
void SerialInitPDA ( uint32_t baudrate )
{
    PORTC.OUTSET = PIN3_bm; //PC3 (TXC0) as output
    PORTC.DIRSET = PIN3_bm;
    PORTC.OUTCLR = PIN2_bm; //PC2 (RXC0) as input
    PORTC.DIRCLR = PIN2_bm;
    Buffer.UART[PDA].port->BAUDCTRLA = ( ( ( ( F_CPU / baudrate ) + 8 ) / 16 ) - 1 ) & 255;
    Buffer.UART[PDA].port->BAUDCTRLB = ( ( ( ( F_CPU / baudrate ) + 8 ) / 16 ) - 1 ) >> 8;
    //Set asynchronous mode
    Buffer.UART[PDA].port->CTRLC |= USART_CMODE_ASYNCHRONOUS_gc;
    Buffer.UART[PDA].port->CTRLC |= USART_CHSIZE_8BIT_gc;
    //Buffer.UART[PDA].port->CTRLA |= USART_RXCINTLVL_LO_gc;
}
void SerialStartPDA()
{
    Buffer.UART[PDA].port->CTRLA |= USART_RXCINTLVL_LO_gc;
    Buffer.UART[PDA].port->CTRLB |= USART_RXEN_bm;
    Buffer.UART[PDA].port->CTRLB |= USART_TXEN_bm;
}
void SerialEndPDA()
{
    Buffer.UART[PDA].port->CTRLA |= USART_RXCINTLVL_OFF_gc;
    Buffer.UART[PDA].port->CTRLB &= ~ ( USART_RXEN_bm );
    Buffer.UART[PDA].port->CTRLB &= ~ ( USART_TXEN_bm );
}


void SerialInitExtTTL ( uint32_t baudrate )
{
    PORTE.OUTSET = PIN7_bm; //PE7 (TXE1) as output
    PORTE.DIRSET = PIN7_bm;
    PORTE.OUTCLR = PIN6_bm; //PE6 (RXE1) as input
    PORTE.DIRCLR = PIN6_bm;
    Buffer.UART[ExtTTL].port->BAUDCTRLA = ( ( ( ( F_CPU / baudrate ) + 8 ) / 16 ) - 1 ) & 255;
    Buffer.UART[ExtTTL].port->BAUDCTRLB = ( ( ( ( F_CPU / baudrate ) + 8 ) / 16 ) - 1 ) >> 8;
    //Set asynchronous mode
    Buffer.UART[ExtTTL].port->CTRLC |= USART_CMODE_ASYNCHRONOUS_gc;
    Buffer.UART[ExtTTL].port->CTRLC |= USART_CHSIZE_8BIT_gc;
    //Buffer.UART[ExtTTL].port->CTRLA |= USART_RXCINTLVL_LO_gc;
}
void SerialStartExtTTL()
{
    Buffer.UART[ExtTTL].port->CTRLA |= USART_RXCINTLVL_LO_gc;
    Buffer.UART[ExtTTL].port->CTRLB |= USART_RXEN_bm;
    Buffer.UART[ExtTTL].port->CTRLB |= USART_TXEN_bm;
}
void SerialEndExtTTL()
{
    Buffer.UART[ExtTTL].port->CTRLA |= USART_RXCINTLVL_OFF_gc;
    Buffer.UART[ExtTTL].port->CTRLB &= ~ ( USART_RXEN_bm );
    Buffer.UART[ExtTTL].port->CTRLB &= ~ ( USART_TXEN_bm );
}

void SerialInitExtRS ( uint32_t baudrate )
{
    PORTF.OUTSET = PIN3_bm; //PF3 (TXF0) as output
    PORTF.DIRSET = PIN3_bm;
    PORTF.OUTCLR = PIN2_bm; //PF2 (RXF0) as input
    PORTF.DIRCLR = PIN2_bm;
    Buffer.UART[ExtRS].port->BAUDCTRLA = ( ( ( ( F_CPU / baudrate ) + 8 ) / 16 ) - 1 ) & 255;
    Buffer.UART[ExtRS].port->BAUDCTRLB = ( ( ( ( F_CPU / baudrate ) + 8 ) / 16 ) - 1 ) >> 8;
    //Set asynchronous mode
    Buffer.UART[ExtRS].port->CTRLC |= USART_CMODE_ASYNCHRONOUS_gc;
    Buffer.UART[ExtRS].port->CTRLC |= USART_CHSIZE_8BIT_gc;
    //Buffer.UART[ExtRS].port->CTRLA |= USART_RXCINTLVL_LO_gc;
}
void SerialStartExtRS()
{
    Buffer.UART[ExtRS].port->CTRLA |= USART_RXCINTLVL_LO_gc;
    Buffer.UART[ExtRS].port->CTRLB |= USART_RXEN_bm;
    Buffer.UART[ExtRS].port->CTRLB |= USART_TXEN_bm;
}
void SerialEndExtRS()
{
    Buffer.UART[ExtRS].port->CTRLA |= USART_RXCINTLVL_OFF_gc;
    Buffer.UART[ExtRS].port->CTRLB &= ~ ( USART_RXEN_bm );
    Buffer.UART[ExtRS].port->CTRLB &= ~ ( USART_TXEN_bm );
}

void SerialInitBT ( uint32_t baudrate )
{
    PORTC.OUTSET = PIN7_bm; //PC7 (TXC1) as output
    PORTC.DIRSET = PIN7_bm;
    PORTC.OUTCLR = PIN6_bm; //PC6 (RXC1) as input
    PORTC.DIRCLR = PIN6_bm;
    Buffer.UART[BT].port->BAUDCTRLA = ( ( ( ( F_CPU / baudrate ) + 8 ) / 16 ) - 1 ) & 255;
    Buffer.UART[BT].port->BAUDCTRLB = ( ( ( ( F_CPU / baudrate ) + 8 ) / 16 ) - 1 ) >> 8;
    //Set asynchronous mode
    Buffer.UART[BT].port->CTRLC |= USART_CMODE_ASYNCHRONOUS_gc;
    Buffer.UART[BT].port->CTRLC |= USART_CHSIZE_8BIT_gc;
    //Buffer.UART[BT].port->CTRLA |= USART_RXCINTLVL_LO_gc;
}
void SerialStartBT()
{
    Buffer.UART[BT].port->CTRLA |= USART_RXCINTLVL_LO_gc;
    Buffer.UART[BT].port->CTRLB |= USART_RXEN_bm;
    Buffer.UART[BT].port->CTRLB |= USART_TXEN_bm;
}
void SerialEndBT()
{
    Buffer.UART[BT].port->CTRLA |= USART_RXCINTLVL_OFF_gc;
    Buffer.UART[BT].port->CTRLB &= ~ ( USART_RXEN_bm );
    Buffer.UART[BT].port->CTRLB &= ~ ( USART_TXEN_bm );
}

void SerialInitGPS ( uint32_t baudrate )
{
    PORTD.OUTSET = PIN3_bm; //PD3 (TXD0) as output
    PORTD.DIRSET = PIN3_bm;
    PORTD.OUTCLR = PIN2_bm; //PD2 (RXD0) as input
    PORTD.DIRCLR = PIN2_bm;
    Buffer.UART[GPS].port->BAUDCTRLA = ( ( ( ( F_CPU / baudrate ) + 8 ) / 16 ) - 1 ) & 255;
    Buffer.UART[GPS].port->BAUDCTRLB = ( ( ( ( F_CPU / baudrate ) + 8 ) / 16 ) - 1 ) >> 8;
    //Set asynchronous mode
    Buffer.UART[GPS].port->CTRLC |= USART_CMODE_ASYNCHRONOUS_gc;
    Buffer.UART[GPS].port->CTRLC |= USART_CHSIZE_8BIT_gc;
    //Buffer.UART[GPS].port->CTRLA |= USART_RXCINTLVL_LO_gc;
}
void SerialStartGPS()
{
    Buffer.UART[GPS].port->CTRLA |= USART_RXCINTLVL_LO_gc;
    Buffer.UART[GPS].port->CTRLB |= USART_RXEN_bm;
    Buffer.UART[GPS].port->CTRLB |= USART_TXEN_bm;
}
void SerialEndGPS()
{
    Buffer.UART[GPS].port->CTRLA |= USART_RXCINTLVL_OFF_gc;
    Buffer.UART[GPS].port->CTRLB &= ~ ( USART_RXEN_bm );
    Buffer.UART[GPS].port->CTRLB &= ~ ( USART_TXEN_bm );
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///SerialDataFrom
///////////////////////////////////////////////////////////////////////////////////////////////////
void SerialDataFrom ( uint8_t source )
{
    SerialEndGPS();
    SerialEndExtTTL();
    SerialEndExtRS();
    SerialEndBT();
    Serial_Enabled_BaroInt = FALSE;
    Serial_Enabled_ExtRS = FALSE;
    Serial_Enabled_GPS = FALSE;
    Serial_Enabled_ExtTTL = FALSE;
    Serial_Enabled_BT = FALSE;
    Serial_Enabled_PDA = TRUE;
    
    switch ( source ) {
    case GPS:
        SerialStartGPS();
        Serial_Enabled_BaroInt = TRUE;
        Serial_Enabled_GPS = TRUE;
        break;
        
    case ExtTTL:
        SerialStartExtTTL();
        Serial_Enabled_BaroInt = FALSE;
        Serial_Enabled_ExtTTL = TRUE;
        break;
        
    case ExtRS:
        SerialStartExtRS();
        Serial_Enabled_BaroInt = FALSE;
        Serial_Enabled_ExtRS = TRUE;
        break;
        
    case BT:
        SerialStartBT();
        Serial_Enabled_BaroInt = FALSE;
        Serial_Enabled_BT = TRUE;
        break;
        
    case AllSerial:
        SerialStartGPS();
        Serial_Enabled_GPS = TRUE;
        SerialStartExtTTL();
        Serial_Enabled_ExtTTL = TRUE;
        SerialStartExtRS();
        Serial_Enabled_ExtRS = TRUE;
        SerialStartBT();
        Serial_Enabled_BT = TRUE;
        
        Serial_Enabled_BaroInt = TRUE;
        break;
    }
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///Buffer_Clear
///////////////////////////////////////////////////////////////////////////////////////////////////
void Buffer_Clear ( uint8_t dir, uint8_t target )
{
    struct BufferPerDirection *buffSer;
    
    if ( dir == BUFFERDIRIN )
        buffSer = &Buffer.UART[target].BufferIn;
        
    else
        buffSer = &Buffer.UART[target].BufferOut;
        
    buffSer->readPos = 0;
    buffSer->writePos = 0;
    buffSer->isEmpty = TRUE;
    buffSer->isFull = FALSE;
    buffSer->dataToSend = FALSE;
    buffSer->isReading = FALSE;
    buffSer->locked = FALSE;
    buffSer->dataSizeInBuffer = 0;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///Buffer_CharIn
///////////////////////////////////////////////////////////////////////////////////////////////////
void Buffer_CharIn ( char c, uint8_t dir, uint8_t target )
{
    struct BufferPerDirection *buffSer;
    
    if ( dir == BUFFERDIRIN )
        buffSer = & Buffer.UART[target].BufferIn;
        
    else
        buffSer = & Buffer.UART[target].BufferOut;
        
    if ( buffSer->isFull == FALSE ) {
        buffSer->Buffer[buffSer->writePos] = c;
        buffSer->dataSizeInBuffer++;
    }
    
    Buffer_WritePosUp ( dir, target );
    Buffer_CheckState ( dir, target );
    
    if ( ( c == '\n' ) && ( dir == BUFFERDIRIN ) )
        buffSer->dataToSend++;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///Buffer_CharOut
///////////////////////////////////////////////////////////////////////////////////////////////////
char Buffer_CharOut ( uint8_t dir, uint8_t target )
{
    char TempChar = 0;
    struct BufferPerDirection *buffSer;
    
    if ( dir == BUFFERDIRIN )
        buffSer = &Buffer.UART[target].BufferIn;
        
    else
        buffSer = &Buffer.UART[target].BufferOut;
        
    if ( buffSer->isEmpty == FALSE ) {
        TempChar = buffSer->Buffer[buffSer->readPos];
        
        if ( buffSer->dataSizeInBuffer > 0 )
            buffSer->dataSizeInBuffer--;
            
        if ( TempChar == '\n' && buffSer->dataToSend > 0 )
            buffSer->dataToSend--;
    }
    
    Buffer_ReadPosUp ( dir, target );
    Buffer_CheckState ( dir, target );
    return TempChar;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///Buffer_CheckChar
///////////////////////////////////////////////////////////////////////////////////////////////////
char Buffer_CheckChar ( uint8_t dir, uint8_t target, uint8_t position )
{
    uint16_t TempReadPos = 0;
    uint8_t i = 0;
    struct BufferPerDirection *buffSer;
    
    if ( dir == BUFFERDIRIN )
        buffSer = &Buffer.UART[target].BufferIn;
        
    else
        buffSer = &Buffer.UART[target].BufferOut;
        
    TempReadPos = buffSer->readPos;
    
    for ( i; i < position; i++ ) {
        TempReadPos++;
        
        if ( TempReadPos >= buffSer->bufferSize )
            TempReadPos = 0;
    }
    
    return buffSer->Buffer[TempReadPos];
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///Buffer_Buffer_CheckNMEA
///////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t Buffer_CheckNMEA ( uint8_t dir, uint8_t target )
{
    char char0;
    char char1;
    char char2;
    char char3;
    char char4;
    char char5;
    
    
    struct BufferPerDirection *buffSer;
    
    if ( dir == BUFFERDIRIN )
        buffSer = &Buffer.UART[target].BufferIn;
        
    else
        buffSer = &Buffer.UART[target].BufferOut;
        
    char0 = Buffer_CheckChar ( dir, target, 0 );
    char1 = Buffer_CheckChar ( dir, target, 1 );
    char2 = Buffer_CheckChar ( dir, target, 2 );
    char3 = Buffer_CheckChar ( dir, target, 3 );
    char4 = Buffer_CheckChar ( dir, target, 4 );
    char5 = Buffer_CheckChar ( dir, target, 5 );
    
    switch ( char0 ) {
    case '$':
        if ( char1 == 'G' && char2 == 'P' && char3 == 'G' && char4 == 'G' && char5 == 'A' )
            return NMEA_GPGGA;
            
        else if ( char1 == 'G' && char2 == 'P' && char3 == 'G' && char4 == 'S' && char5 == 'A' )
            return NMEA_GPGSA;
            
        else if ( char1 == 'G' && char2 == 'P' && char3 == 'G' && char4 == 'S' && char5 == 'V' )
            return NMEA_GPGSV;
            
        else if ( char1 == 'G' && char2 == 'P' && char3 == 'R' && char4 == 'M' && char5 == 'C' )
            return NMEA_GPRMC;
            
        else if ( char1 == 'G' && char2 == 'P' && char3 == 'V' && char4 == 'T' && char5 == 'G' )
            return NMEA_GPVTG;
            
        else if ( char1 == 'P' && char2 == 'G' && char3 == 'R' && char4 == 'M' && char5 == 'Z' )
            return NMEA_PGRMZ;
            
        else if ( char1 == 'P' && char2 == 'M' && char3 == 'T' && char4 == 'K' )
            return NMEA_PMTK;
            
        else if ( char1 == 'P' && char2 == 'S' && char3 == 'C' && char4 == 'S' && char5 == 'O' )
            return NMEA_PSCSO;
            
        else if ( char1 == 'P' && char2 == 'S' && char3 == 'C' && char4 == 'S' && char5 == 'W' )
            return NMEA_PSCSW;
            
        else
            return NMEA_UNKNOWNNMEA;
            
        break;
        
    case 'O' :
        if ( char1 == 'K' )
            return AT_OK;
            
        break;
        
    default:
        return NMEA_NONMEA;
    }
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///Buffer_StringIn
///////////////////////////////////////////////////////////////////////////////////////////////////
void Buffer_StringIn ( char *string, uint8_t length, uint8_t dir, uint8_t target )
{
    for ( int i = 0; i < length; i++ ) {
        Buffer_CharIn ( *string++, dir, target );
    }
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///Buffer_StringOut
///////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t Buffer_StringOut ( char *string, uint8_t dir, uint8_t target )
{
    uint8_t length = 0;
    
    struct BufferPerDirection *buffSer;
    
    if ( dir == BUFFERDIRIN )
        buffSer = &Buffer.UART[target].BufferIn;
        
    else
        buffSer = &Buffer.UART[target].BufferOut;
        
    while ( buffSer->dataSizeInBuffer > 0 ) {
        *string = Buffer_CharOut ( dir, target );
        *string++;
        length++;
    }
    
    return length;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///Buffer_DataOut
///////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t Buffer_DataOut ( char *string, uint8_t length, uint8_t dir, uint8_t target )
{
    int i = 0;
    struct BufferPerDirection *buffSer;
    //make sure that no old data are there
    //*string = 0;
    
    if ( dir == BUFFERDIRIN )
        buffSer = &Buffer.UART[target].BufferIn;
        
    else
        buffSer = &Buffer.UART[target].BufferOut;
        
    //Check if a datapacket is ready
    if ( buffSer->dataToSend > 0 ) {
        //Read until /n
        do {
            *string = Buffer_CharOut ( dir, target );
        }
        
        while ( *string++ != '\n' && i++ < length );
        
        i++;
    }
    
    return i;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///Buffer_WritePosUp
///////////////////////////////////////////////////////////////////////////////////////////////////
void Buffer_WritePosUp ( uint8_t dir, uint8_t target )
{
    struct BufferPerDirection *buffSer;
    
    if ( dir == BUFFERDIRIN )
        buffSer = &Buffer.UART[target].BufferIn;
        
    else
        buffSer = &Buffer.UART[target].BufferOut;
        
    //Writepos 1++
    if ( buffSer->isFull == FALSE ) {
        //Writepos in last position
        if ( buffSer->writePos == ( buffSer->bufferSize ) - 1 )
            buffSer->writePos = 0;
            
        //Writepos not in last position
        else
            buffSer->writePos++;
    }
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///Buffer_ReadPosUp
///////////////////////////////////////////////////////////////////////////////////////////////////
void Buffer_ReadPosUp ( uint8_t dir, uint8_t target )
{
    struct BufferPerDirection *buffSer;
    
    if ( dir == BUFFERDIRIN )
        buffSer = &Buffer.UART[target].BufferIn;
        
    else
        buffSer = &Buffer.UART[target].BufferOut;
        
    //Readposition 1++
    if ( buffSer->isEmpty == FALSE ) {
        //Readposition in last position
        if ( buffSer->readPos == ( buffSer->bufferSize ) - 1 )
            buffSer->readPos = 0;
            
        //Readposition not in last position
        else
            buffSer->readPos++;
    }
    
    //if /n, one datapacket less
    //if ( buffSer->Buffer[buffSer->readPos] == '/n' )
    //    buffSer->dataToSend--;
}




///////////////////////////////////////////////////////////////////////////////////////////////////
///Buffer_CheckState
///////////////////////////////////////////////////////////////////////////////////////////////////
void Buffer_CheckState ( uint8_t dir, uint8_t target )
{
    struct BufferPerDirection *buffSer;
    
    if ( dir == BUFFERDIRIN )
        buffSer = &Buffer.UART[target].BufferIn;
        
    else
        buffSer = &Buffer.UART[target].BufferOut;
        
    //Check if Buffer is empty
    if ( buffSer->writePos == buffSer->readPos ) {
        buffSer->isEmpty = TRUE;
        buffSer->isFull = FALSE;
    }
    
    else
        buffSer->isEmpty = FALSE;
        
    //Check if Buffer is full
    if ( buffSer->isEmpty == FALSE ) {
        //Readpos
        if ( buffSer->writePos == buffSer->bufferSize - 1 ) {
            if ( buffSer->readPos == 0 ) {
                buffSer->isFull = TRUE;
                Buffer_Clear ( dir, target );
                Serial_State = Serial_Error_BufferOverFlow;
            }
        }
        
        else {
            if ( buffSer->readPos == buffSer->writePos + 1 ) {
                buffSer->isFull = TRUE;
                Buffer_Clear ( dir, target );
                Serial_State = Serial_Error_BufferOverFlow;
            }
            
            else
                buffSer->isFull = FALSE;
				Serial_State=Serial_Error_NoError;
        }
    }
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///Buffer_MoveData
///////////////////////////////////////////////////////////////////////////////////////////////////
void Buffer_MoveData ( uint8_t source, uint8_t target )
{
    char tempString[BUFFERTEMPLENGTH];
    uint8_t tempStringLength = 0;
    
    tempStringLength = Buffer_DataOut ( tempString, BUFFERTEMPLENGTH, BUFFERDIRIN, source );
    Buffer_StringIn ( tempString, tempStringLength, BUFFERDIROUT, target );
    Buffer.UART[target].port->CTRLA |= USART_DREINTLVL_LO_gc;
    Buffer.UART[target].BufferOut.isEmpty = FALSE;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///SerialManager
///////////////////////////////////////////////////////////////////////////////////////////////////
void SerialManager ()
{
    char _databyte;
    char tempString[BUFFERTEMPLENGTH];
    uint8_t tempStringLength = 0;
    
    //Check all In buffers if there are data ready to send
    for ( int i = 0; i < BufferCount; i++ ) {
        //if ( ( Buffer.UART[i].BufferIn.isEmpty == FALSE ) && ( Buffer.UART[i].BufferIn.dataToSend > 0 ) && ( i != PDA ) ) {
        if ( ( Buffer.UART[i].BufferIn.isEmpty == FALSE ) && ( Buffer.UART[i].BufferIn.dataToSend > 0 ) ) {
            //as long there are data packets
            do {
                //Check wich code
                switch ( Buffer_CheckNMEA ( BUFFERDIRIN, i ) ) {
                case NMEA_GPGGA:
                    Buffer_MoveData ( i, PDA );
                    break;
                    
                case NMEA_GPGSA:
                    Buffer_MoveData ( i, PDA );
                    break;
                    
                case NMEA_GPGSV:
                    Buffer_MoveData ( i, PDA );
                    break;
                    
                case NMEA_GPRMC:
                    Buffer_MoveData ( i, PDA );
                    break;
                    
                case NMEA_GPVTG:
                    Buffer_MoveData ( i, PDA );
                    break;
                    
                case  NMEA_PGRMZ:
                    Buffer_MoveData ( i, PDA );
                    break;
                    
                case NMEA_PMTK:
                    Buffer_MoveData ( i, PDA );
                    break;
                    
                case  NMEA_PSCSO:
                    //Buffer_MoveData ( i, PDA );
                    tempStringLength = Buffer_DataOut ( tempString, BUFFERTEMPLENGTH, BUFFERDIRIN, i );
                    
                    //Chose sound
                    switch ( tempString[7] ) {
                    case '0': //Startup
                        Buzzer_PlayMelody ( melodyStartup );
                        break;
                        
                    case '1': //Beep1
                        Buzzer_PlayMelody ( melodyBeep1 );
                        break;
                        
                    case '2': //Beep2
                        Buzzer_PlayMelody ( melodyBeep2 );
                        break;
                        
                    case '3': //Beep3
                        Buzzer_PlayMelody ( melodyBeep3 );
                        break;
                        
                    case '4': //Beep4
                        Buzzer_PlayMelody ( melodyBeep4 );
                        break;
                        
                    case '5': //Beep5
                        Buzzer_PlayMelody ( melodyBeep5 );
                        break;
                        
                    case '6': //Warning
                        Buzzer_PlayMelody ( melodyWarning );
                        break;
                        
                    case '7': //Airspace
                        Buzzer_PlayMelody ( melodyAirspace );
                        break;
                        
                    case '8': //Flarm In
                        Buzzer_PlayMelody ( melodyFlarmIn );
                        break;
                        
                    case '9': //Flarm Out
                        Buzzer_PlayMelody ( melodyFlarmOut );
                        break;
					
					case '10': //Message
						Buzzer_PlayMelody ( melodyMessage );
						break;
					
					case '11': //Message
						Buzzer_PlayMelody ( melodyTask );
						break;	
						
                    }
                    
                    break;
                    
                case NMEA_PSCSW:
                    Buffer_MoveData ( i, PDA );
                    break;
                    
                case NMEA_UNKNOWNNMEA:
                    Buffer_MoveData ( i, PDA );
                    break;
                    
                case NMEA_NONMEA:
                    tempStringLength = Buffer_DataOut ( tempString, BUFFERTEMPLENGTH, BUFFERDIRIN, i );
                    //ToggleOutput ( LED_Blue );
                    //Buffer_MoveData ( i, PDA );
                    break;
                    
                default:
                    //Remove unknown data
                    tempStringLength = Buffer_DataOut ( tempString, BUFFERTEMPLENGTH, BUFFERDIRIN, i );
                    
                }
            } while ( Buffer.UART[i].BufferIn.dataToSend > 0 );
        }
        
        //Data from PDA, sent to device
        if ( ( Buffer.UART[i].BufferIn.isEmpty == FALSE ) && ( Buffer.UART[i].BufferIn.dataToSend > 0 ) && ( i == PDA ) ) {
            //as long there are data packets
            do {
                Buffer_MoveData ( i, ExtTTL );
            } while ( Buffer.UART[i].BufferIn.dataToSend > 0 );
        }
    }
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///SetupGPS
///////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t SetupGPS ( uint32_t baudrate, uint8_t refreshrate )
{
    //Set Baudraute
    switch ( baudrate ) {
    case 9600:
        Buffer_StringIn ( "$PMTK251,9600*17\r\n", 18, BUFFERDIROUT, GPS );
        Buffer.UART[GPS].port->CTRLA |= USART_DREINTLVL_LO_gc;
        Buffer.UART[GPS].BufferOut.isEmpty = FALSE;
        break;
        
    case 19200:
        Buffer_StringIn ( "$PMTK251,19200*22\r\n", 19, BUFFERDIROUT, GPS );
        Buffer.UART[GPS].port->CTRLA |= USART_DREINTLVL_LO_gc;
        Buffer.UART[GPS].BufferOut.isEmpty = FALSE;
        break;
        
    case 38400:
        Buffer_StringIn ( "$PMTK251,38400*27\r\n", 19, BUFFERDIROUT, GPS );
        Buffer.UART[GPS].port->CTRLA |= USART_DREINTLVL_LO_gc;
        Buffer.UART[GPS].BufferOut.isEmpty = FALSE;
        break;
        
    case 57600:
        Buffer_StringIn ( "$PMTK251,57600*2C\r\n", 19, BUFFERDIROUT, GPS );
        Buffer.UART[GPS].port->CTRLA |= USART_DREINTLVL_LO_gc;
        Buffer.UART[GPS].BufferOut.isEmpty = FALSE;
        break;
        
    case 115200:
        Buffer_StringIn ( "$PMTK251,115200*1F\r\n", 20, BUFFERDIROUT, GPS );
        Buffer.UART[GPS].port->CTRLA |= USART_DREINTLVL_LO_gc;
        Buffer.UART[GPS].BufferOut.isEmpty = FALSE;
        break;
        
    default:
        return FALSE;
    }
    
    _delay_ms ( 100 );
    
    //Change Baudrate Soarcase
    SerialInitGPS ( baudrate );
    
    _delay_ms ( 100 );
    
    //Change refreshrate
    switch ( refreshrate ) {
    case 1:
        Buffer_StringIn ( "$PMTK220,1000*1F\r\n", 18, BUFFERDIROUT, GPS );
        Buffer.UART[GPS].port->CTRLA |= USART_DREINTLVL_LO_gc;
        Buffer.UART[GPS].BufferOut.isEmpty = FALSE;
        break;
        
    case 5:
        Buffer_StringIn ( "$PMTK220,200*2C\r\n", 17, BUFFERDIROUT, GPS );
        Buffer.UART[GPS].port->CTRLA |= USART_DREINTLVL_LO_gc;
        Buffer.UART[GPS].BufferOut.isEmpty = FALSE;
        break;
        
    case 10:
        Buffer_StringIn ( "$PMTK220,100*2F\r\n", 17, BUFFERDIROUT, GPS );
        Buffer.UART[GPS].port->CTRLA |= USART_DREINTLVL_LO_gc;
        Buffer.UART[GPS].BufferOut.isEmpty = FALSE;
        break;
        
    default:
        return FALSE;
    }
    
    //Set Nav speed threshold to 0
    Buffer_StringIn ( "$PMTK397,0.0*3D\r\n", 17, BUFFERDIROUT, GPS );
    Buffer.UART[GPS].port->CTRLA |= USART_DREINTLVL_LO_gc;
    Buffer.UART[GPS].BufferOut.isEmpty = FALSE;
    
    return TRUE;
}


uint8_t SetupBT ( void )
{
    char tempString[15];
    uint8_t lengthtempString = 0;
    
    //Set baud to max. and test if this is ok, else we have to setup the bt modul
    SerialInitBT ( Baudrate.Baud57600 );
    
    //Send AT (for testing)
    Buffer_StringIn ( "AT", 2, BUFFERDIROUT, BT );
    Buffer.UART[BT].port->CTRLA |= USART_DREINTLVL_LO_gc;
    Buffer.UART[BT].BufferOut.isEmpty = FALSE;
    
    //Check OK
    _delay_ms ( 500 );
    
    if ( Buffer.UART[BT].BufferIn.dataSizeInBuffer >= 2 ) {
        lengthtempString = Buffer_StringOut ( &tempString, BUFFERDIRIN, BT );
        
        if ( tempString[0] == 'O' && tempString[1] == 'K' )
            Buffer_Clear ( BUFFERDIRIN, BT );
            
        else return FALSE;
        
    } else {
        //Baudrate not correct
        SerialInitBT ( Baudrate.Baud9600 );
        _delay_ms ( 100 );
        
        //Send AT to change Baudrate
        Buffer_StringIn ( "AT+BAUD7", 8, BUFFERDIROUT, BT );
        Buffer.UART[BT].port->CTRLA |= USART_DREINTLVL_LO_gc;
        Buffer.UART[BT].BufferOut.isEmpty = FALSE;
        
        _delay_ms ( 800 );
        SerialInitBT ( Baudrate.Baud57600 );
        _delay_ms ( 100 );
        
        //Set name
        Buffer_StringIn ( "AT+NAMESoarCaseV1", 17, BUFFERDIROUT, BT );
        Buffer.UART[BT].port->CTRLA |= USART_DREINTLVL_LO_gc;
        Buffer.UART[BT].BufferOut.isEmpty = FALSE;
        
        //Set Master role
        Buffer_StringIn ( "AT+ROLEM", 8, BUFFERDIROUT, BT );
        Buffer.UART[BT].port->CTRLA |= USART_DREINTLVL_LO_gc;
        Buffer.UART[BT].BufferOut.isEmpty = FALSE;
    }
    
    //Set Master role
    Buffer_StringIn ( "AT+ROLEM", 8, BUFFERDIROUT, BT );
    Buffer.UART[BT].port->CTRLA |= USART_DREINTLVL_LO_gc;
    Buffer.UART[BT].BufferOut.isEmpty = FALSE;
    
    _delay_ms ( 10000 );
    
    //Connect to HB-1845
    //Buffer_StringIn ( "AT+LNK000EEACF4717", 18, BUFFERDIROUT, BT );
    //Buffer_StringIn ( "AT+SCAN?", 8, BUFFERDIROUT, BT );
    //Buffer_StringIn ( "AT+VERSION?", 11, BUFFERDIROUT, BT );
    //Buffer_StringIn ( "AT+ADDR?", 8, BUFFERDIROUT, BT );
    //Buffer_StringIn ( "AT+SEARCH", 9, BUFFERDIROUT, BT );
    //Buffer_StringIn ( "AT+RADD?", 8, BUFFERDIROUT, BT );
    Buffer_StringIn ( "AT+HELP?", 8, BUFFERDIROUT, BT );
    Buffer.UART[BT].port->CTRLA |= USART_DREINTLVL_LO_gc;
    Buffer.UART[BT].BufferOut.isEmpty = FALSE;
    
    //_delay_ms ( 5000 );
    
    return TRUE;
}