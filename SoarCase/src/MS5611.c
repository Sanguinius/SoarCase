#include "MS5611.h"

#define MS5611_Adress1 0X77					//CSB Pin low
#define MS5611_Adress2 0x76					//CSB Pin high


//Tropospheric properties (0-11km) for standard atmosphere
const double Tb = 288.15;					//Temperature at base height in Kelvin
const double Lb  = -0.0065;					//Temperature gradient in degrees per metre
const double g  = 9.80665;					//Gravity constant in m/s/s
const double R = 8.31432;					//Unversal gas constant in N*m/mol*K
const double hb = 0.0;						//Height at bottom of atmospheric layer in m
const double M = 0.0289644;					//Molar mass of earth's air in kg/mol

uint8_t MS5611_CMD_Reset = 0x1E	;			//00011110
uint8_t MS5611_CMD_ADCRead = 0x00;			//00000000

uint8_t MS5611_CMD_D1_BaseAdress = 0x40;	//01000000
uint8_t MS5611_CMD_D2_BaseAdress = 0x50;	//01010000

uint8_t MS5611_CMD_PROMRead_C1 = 0xA2;		//1010 001 0
uint8_t MS5611_CMD_PROMRead_C2 = 0xA4;		//1010 010 0
uint8_t MS5611_CMD_PROMRead_C3 = 0xA6;		//1010 011 0
uint8_t MS5611_CMD_PROMRead_C4 = 0xA8;		//1010 100 0
uint8_t MS5611_CMD_PROMRead_C5 = 0xAA;		//1010 101 0
uint8_t MS5611_CMD_PROMRead_C6 = 0xAC;		//1010 110 0

double MS5611_QNH = 1013.25;				//Standard pressure in hPa as inital value


///////////////////////////////////////////////////////////////////////////////////////////////////
///MS56511_Init
///////////////////////////////////////////////////////////////////////////////////////////////////
bool MS5611_Init ( uint8_t OSR_Mode )
{
    if ( MS5611_DataInit ( OSR_Mode ) != SUCCESS )
        return ERROR;

    //C1
    if ( TWI_MasterWriteRead ( &twiMaster, MS5611.adress, &MS5611_CMD_PROMRead_C1, 1, 2 ) != SUCCESS )
        return ERROR;

    while ( twiMaster.status != TWIM_STATUS_READY );

    MS5611.data.c[1] = twiMaster.readData[0] << 8 | twiMaster.readData[1];

    //C2
    if ( TWI_MasterWriteRead ( &twiMaster, MS5611.adress, &MS5611_CMD_PROMRead_C2, 1, 2 ) != SUCCESS )
        return ERROR;

    while ( twiMaster.status != TWIM_STATUS_READY );

    MS5611.data.c[2] = twiMaster.readData[0] << 8 | twiMaster.readData[1];

    //C3
    if ( TWI_MasterWriteRead ( &twiMaster, MS5611.adress, &MS5611_CMD_PROMRead_C3, 1, 2 ) != SUCCESS )
        return ERROR;

    while ( twiMaster.status != TWIM_STATUS_READY );

    MS5611.data.c[3] = twiMaster.readData[0] << 8 | twiMaster.readData[1];

    //C4
    if ( TWI_MasterWriteRead ( &twiMaster, MS5611.adress, &MS5611_CMD_PROMRead_C4, 1, 2 ) != SUCCESS )
        return ERROR;

    while ( twiMaster.status != TWIM_STATUS_READY );

    MS5611.data.c[4] = twiMaster.readData[0] << 8 | twiMaster.readData[1];

    //C5
    if ( TWI_MasterWriteRead ( &twiMaster, MS5611.adress, &MS5611_CMD_PROMRead_C5, 1, 2 ) != SUCCESS )
        return ERROR;

    while ( twiMaster.status != TWIM_STATUS_READY );

    MS5611.data.c[5] = twiMaster.readData[0] << 8 | twiMaster.readData[1];

    //C6
    if ( TWI_MasterWriteRead ( &twiMaster, MS5611.adress, &MS5611_CMD_PROMRead_C6, 1, 2 ) != SUCCESS )
        return ERROR;

    while ( twiMaster.status != TWIM_STATUS_READY );

    MS5611.data.c[6] = twiMaster.readData[0] << 8 | twiMaster.readData[1];

    //Init standard pressure
    MS5611.pressureCorrection = MS5611_QNH;

    MS5611.status = MS5611_STATUS_IDLE;
    return SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///MS56511_GetData
///Should be called every 10ms (not faster). Acts like a state machine
///////////////////////////////////////////////////////////////////////////////////////////////////
bool MS5611_GetData()
{
    //Sensor idle, start temperature conversion (Only when first start)
    if ( MS5611.status == MS5611_STATUS_IDLE ) {
        uint8_t cmd = MS5611_CMD_D2_BaseAdress + ( MS5611.OSR_Mode << 1 ) ;

        if ( TWI_MasterWrite ( &twiMaster, MS5611.adress, &cmd, 1 ) != SUCCESS ) {
            MS5611.status = MS5611_STATUS_ERROR;
            return ERROR;
        }

        while ( twiMaster.status != TWIM_STATUS_READY );

        MS5611.status = MS5611_STATUS_WAITD2;
        return SUCCESS;
    }

    //Temperatur conversion ready, now read data from sensor and start pressure conversion
    if ( MS5611.status == MS5611_STATUS_WAITD2 ) {
        if ( TWI_MasterWriteRead ( &twiMaster, MS5611.adress, &MS5611_CMD_ADCRead, 1, 3 ) != SUCCESS ) {
            MS5611.status = MS5611_STATUS_ERROR;
            return ERROR;
        }

        while ( twiMaster.status != TWIM_STATUS_READY );

        MS5611.data.d2 = ( uint32_t ) twiMaster.readData[0] << 16 | ( uint32_t ) twiMaster.readData[1] << 8 | ( uint32_t ) twiMaster.readData[2];

        MS5611.status = MS5611_STATUS_D2OK;
    }

    if ( MS5611.status == MS5611_STATUS_D2OK ) {
        uint8_t cmd = MS5611_CMD_D1_BaseAdress + ( MS5611.OSR_Mode << 1 ) ;

        if ( TWI_MasterWrite ( &twiMaster, MS5611.adress, &cmd, 1 ) != SUCCESS ) {
            MS5611.status = MS5611_STATUS_ERROR;
            return ERROR;
        }

        while ( twiMaster.status != TWIM_STATUS_READY );

        MS5611.status = MS5611_STATUS_WAITD1;
        return SUCCESS;
    }

    //Pressure conversion finished, now read data from sensor and then make the calculations
    if ( MS5611.status == MS5611_STATUS_WAITD1 ) {
        if ( TWI_MasterWriteRead ( &twiMaster, MS5611.adress, &MS5611_CMD_ADCRead, 1, 3 ) != SUCCESS ) {
            MS5611.status = MS5611_STATUS_ERROR;
            return ERROR;
        }

        while ( twiMaster.status != TWIM_STATUS_READY );

        MS5611.data.d1 = ( uint32_t ) twiMaster.readData[0] << 16 | ( uint32_t ) twiMaster.readData[1] << 8 | ( uint32_t ) twiMaster.readData[2];

        MS5611.status = MS5611_STATUS_D1OK;
    }

    if ( MS5611.status == MS5611_STATUS_D1OK ) {
        if ( MS5611_CalcData() != SUCCESS ) {
            MS5611.status = MS5611_STATUS_ERROR;
            return ERROR;
        }

        MS5611.NewData = TRUE;
        MS5611.status = MS5611_STATUS_IDLE;
    }

    //Sensor idle, start temperature conversion
    if ( MS5611.status == MS5611_STATUS_IDLE ) {
        uint8_t cmd = MS5611_CMD_D2_BaseAdress + ( MS5611.OSR_Mode << 1 ) ;

        if ( TWI_MasterWrite ( &twiMaster, MS5611.adress, &cmd, 1 ) != SUCCESS ) {
            MS5611.status = MS5611_STATUS_ERROR;
            return ERROR;
        }

        while ( twiMaster.status != TWIM_STATUS_READY );

        MS5611.status = MS5611_STATUS_WAITD2;
        return SUCCESS;
    }

    //Should not end here
    return ERROR;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///MS56511_SetQNH
///////////////////////////////////////////////////////////////////////////////////////////////////
extern void MS5611_SetQNH ( double qnh )
{
    MS5611_QNH = qnh;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///MS56511_GetVS (Return vertical speed in cm/s
///////////////////////////////////////////////////////////////////////////////////////////////////
extern uint32_t MS5611_GetVS ( void )
{
    return MS5611.data.vario;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///MS56511_DataInit
///////////////////////////////////////////////////////////////////////////////////////////////////
bool MS5611_DataInit ( uint8_t OSR_Mode )
{


    MS5611.adress = MS5611_Adress1;

    if ( 0 <= OSR_Mode && OSR_Mode <= 4 )
        MS5611.OSR_Mode = OSR_Mode;
    else {
        MS5611.status = MS5611_STATUS_ERROR;
        return ERROR;
    }

    MS5611.initialized = false;
    MS5611.status = MS5611_STATUS_UNINIT;
    MS5611.NewData = FALSE;

    for ( uint8_t i = 0; i++; i = 7 )
        MS5611.data.c[i] = 0;

    MS5611.data.d1 = 0;
    MS5611.data.d2 = 0;
    MS5611.data.pressure_raw = 0;
    MS5611.data.temperature = 0;
    MS5611.data.filterFactor = HEIGHTFILTERFACTOR;

    MS5611.data.sum = 0;
    MS5611.data.vario = 0;
    MS5611.data.varioStoreWrite = 0;

    for ( uint8_t j = 0; j++; j = VARIOBUFFERLENGTH )
        MS5611.data.varioBuffer[j] = 0.0;

    return SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///MS56511_CalcData
///////////////////////////////////////////////////////////////////////////////////////////////////
bool MS5611_CalcData ()
{
    int64_t T2;
    int64_t off;
    int64_t off2;
    int64_t sense;
    int64_t sense2;

    //Calculate temperature
    int32_t dT = ( int32_t ) MS5611.data.d2 - ( ( int32_t ) MS5611.data.c[5] << 8 );
    MS5611.data.temperature = 2000 + ( ( dT * ( int64_t ) MS5611.data.c[6] ) >> 23 ) ;

    //Calculate second order temp compensation
    if ( MS5611.data.temperature < 2000 ) {
        //Low temperature
        T2 = ( pow ( ( int64_t ) dT, 2 ) ) / pow ( 2, 31 );
        off2 = 5 * pow ( MS5611.data.temperature - 2000 , 2 ) / 2;
        sense2 = 5 * pow ( MS5611.data.temperature - 2000, 2 ) / 4;

        if ( MS5611.data.temperature < -1500 ) {
            //Very low temperature
            off2 = off2 + 7 * pow ( MS5611.data.temperature + 1500, 2 );
            sense2 = sense2 + 11 * pow ( MS5611.data.temperature + 1500, 2 ) / 2;
        }
    }

    else {
        T2 = 0;
        off2 = 0;
        sense2 = 0;
    }

    //Correction of parameters because second order
    MS5611.data.temperature = MS5611.data.temperature - T2;

    //Calculate temperature compensated pressure
    off = ( ( int64_t ) MS5611.data.c[2]  << 16 ) + ( ( int64_t ) MS5611.data.c[4] * dT  >> 7 );
    off = off - off2;
    sense = ( ( int64_t ) MS5611.data.c[1] << 15 ) + ( ( ( int64_t ) MS5611.data.c[3] * dT ) >> 8 );
    sense = sense - sense2;
    MS5611.data.pressure_raw = 10 * ( ( ( MS5611.data.d1 * sense >> 21 ) - off ) >> 15 );

    //Average pressure to smooth the result
    MS5611_PressureAverage();

    //Copy altitude to altitudeOld before calculate the new altitude
    MS5611.data.altitudeOld = MS5611.data.altitude;

    //Calculate height in centimeters
    MS5611.data.altitude = ( uint32_t ) ( MS5611_CalcHeight ( MS5611.data.pressure_filtered ) * 100 );

    //Calculate Vario parameters
    MS5611_CalcVario ( );

    return SUCCESS;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///MS56511_CalcHeight
///////////////////////////////////////////////////////////////////////////////////////////////////
double MS5611_CalcHeight ( int64_t pressure )
{
    double height;

    //Current pressure at MSL in dPa
    double pb = MS5611_QNH * 1000.0;

    //Measured pressure in dPa
    double p = ( double ) pressure;

    //Altitude
    height = hb + ( Tb / Lb ) * ( pow ( ( p / pb ), ( ( -R * Lb ) / ( g * M ) ) ) - 1.0 );

    return height;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///MS56511_CalcVario
///////////////////////////////////////////////////////////////////////////////////////////////////
void MS5611_CalcVario ( void )
{
    //Check Write position
    if ( ++MS5611.data.varioStoreWrite >=  VARIOBUFFERLENGTH )
        MS5611.data.varioStoreWrite = 0;

    //Calculate actual vario value
    MS5611.data.varioBuffer[MS5611.data.varioStoreWrite] = ( ( double ) MS5611.data.altitude - ( double ) MS5611.data.altitudeOld ) / 0.019335 ;

    //Add newest vario value to the sum
    MS5611.data.varioSum = MS5611.data.varioSum + MS5611.data.varioBuffer[MS5611.data.varioStoreWrite];

    //Remove oldest vario value from the sum
    if ( MS5611.data.varioStoreWrite != ( VARIOBUFFERLENGTH - 1 ) )
        MS5611.data.varioSum = MS5611.data.varioSum - MS5611.data.varioBuffer[MS5611.data.varioStoreWrite + 1];
    else
        MS5611.data.varioSum = MS5611.data.varioSum - MS5611.data.varioBuffer[0];

    //Calculate average
    MS5611.data.vario =  MS5611.data.varioSum / ( VARIOBUFFERLENGTH - 1 );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///MS56511_AverageHeight
///////////////////////////////////////////////////////////////////////////////////////////////////
void MS5611_PressureAverage ( void )
{
    MS5611.data.sum = MS5611.data.sum - MS5611.data.pressure_filtered + ( MS5611.data.pressure_raw );

    MS5611.data.pressure_filtered = ( MS5611.data.sum + ( 1 << ( MS5611.data.filterFactor - 1 ) ) )  >> ( MS5611.data.filterFactor );
}