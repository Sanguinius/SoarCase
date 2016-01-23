#ifndef BMP085_H_
#define BMP085_H_

#include <avr/io.h>
#include <util/delay.h>
#include "Globals.h"
#include "BoardInit.h"
#include "TWI.h"




struct BMP085_Data_struct
{
    uint16_t Temp;
    uint16_t Pressure;
    uint16_t UT;
    uint32_t UP;
    int32_t X1;
    int32_t X2;
    int32_t X3;
    int32_t B3;
    int32_t B4;
    int32_t B5;
    int32_t B6;
    int32_t B7;
} BMP085_Data;

bool BMP085_Init();
bool BMP085_GetData ( uint8_t ossMode );


bool BMP085_GetUT();
bool BMP085_GetUP ( uint8_t ossMode );


#endif /* BMP085_H_ */