#include "BMP085.h"


#define BMP085_Adress 0x77
#define BMP085_RegisterAdress 0xF4
#define BMP085_WaitTimeMeasureUT 4
#define BMP085_WaitTimePressureOSRS0 5
#define BMP085_WaitTimePressureOSRS1 8
#define BMP085_WaitTimePressureOSRS2 14
#define BMP085_WaitTimePressureOSRS3 19


//uint8_t BMP085_RegisterAdress = 0xF4;
uint8_t BMP085_CtrlRegTemp[2] = {BMP085_RegisterAdress, 0x2E};
uint8_t BMP085_CtrlRegPressureOSS0[2] = {BMP085_RegisterAdress, 0x34};
uint8_t BMP085_CtrlRegPressureOSS1[2] = {BMP085_RegisterAdress, 0x74};
uint8_t BMP085_CtrlRegPressureOSS2[2] = {BMP085_RegisterAdress, 0xB4};
uint8_t BMP085_CtrlRegPressureOSS3[2] = {BMP085_RegisterAdress, 0xF4};

uint8_t BMP085_StartAdressCoefficients1_4 = 0xAA;
uint8_t BMP085_StartAdressCoefficients5_8 = 0xB2;
uint8_t BMP085_StartAdressCoefficients9_11 = 0xBA;
uint8_t BMP085_StartAdressTemp = 0xF6;
uint8_t BMP085_StartAdressPressure = 0xF6;


struct BMP085_Coefficients_struct
{
    int16_t AC1;
    int16_t AC2;
    int16_t AC3;
    uint16_t AC4;
    uint16_t AC5;
    uint16_t AC6;
    int16_t B1;
    int16_t B2;
    int16_t MB;
    int16_t MC;
    int16_t MD;
} BMP085_Coefficients;



bool BMP085_Init()
{
    //Fill Coefficients with standard values (from Data sheet)
    BMP085_Coefficients.AC1 = 408;
    BMP085_Coefficients.AC2 = -72;
    BMP085_Coefficients.AC3 = -14383;
    BMP085_Coefficients.AC4 = 32741;
    BMP085_Coefficients.AC5 = 32757;
    BMP085_Coefficients.AC6 = 23153;
    BMP085_Coefficients.B1 = 6190;
    BMP085_Coefficients.B2 = 4;
    BMP085_Coefficients.MB = -32767;
    BMP085_Coefficients.MC = -8711;
    BMP085_Coefficients.MD = 2868;

    //Read AC1 - AC4 coefficients from sensor
    if ( TWI_MasterWriteRead ( &twiMaster, BMP085_Adress, &BMP085_StartAdressCoefficients1_4, 1, 8 ) == SUCCESS )
    {
        while ( twiMaster.status != TWIM_STATUS_READY );

        BMP085_Coefficients.AC1 = twiMaster.readData[0] << 8 | twiMaster.readData[1];
        BMP085_Coefficients.AC2 = twiMaster.readData[2] << 8 | twiMaster.readData[3];
        BMP085_Coefficients.AC3 = twiMaster.readData[4] << 8 | twiMaster.readData[5];
        BMP085_Coefficients.AC4 = twiMaster.readData[6] << 8 | twiMaster.readData[7];
    }

    else return ERROR;

    //Read AC5 - B2 coefficients from sensor
    if ( TWI_MasterWriteRead ( &twiMaster, BMP085_Adress, &BMP085_StartAdressCoefficients5_8, 1, 8 ) == SUCCESS )
    {
        while ( twiMaster.status != TWIM_STATUS_READY );

        BMP085_Coefficients.AC5 = twiMaster.readData[0] << 8 | twiMaster.readData[1];
        BMP085_Coefficients.AC6 = twiMaster.readData[2] << 8 | twiMaster.readData[3];
        BMP085_Coefficients.B1 = twiMaster.readData[4] << 8 | twiMaster.readData[5];
        BMP085_Coefficients.B2 = twiMaster.readData[6] << 8 | twiMaster.readData[7];
    }

    else return ERROR;

    //Read MB - MD coefficients from sensor
    if ( TWI_MasterWriteRead ( &twiMaster, BMP085_Adress, &BMP085_StartAdressCoefficients9_11, 1, 6 ) == SUCCESS )
    {
        while ( twiMaster.status != TWIM_STATUS_READY );

        BMP085_Coefficients.MB = twiMaster.readData[0] << 8 | twiMaster.readData[1];
        BMP085_Coefficients.MC = twiMaster.readData[2] << 8 | twiMaster.readData[3];
        BMP085_Coefficients.MD = twiMaster.readData[4] << 8 | twiMaster.readData[5];
    }

    else return ERROR;

    //Everything OK
    return SUCCESS;
}


bool BMP085_GetData ( uint8_t ossMode )
{
    if ( BMP085_GetUT() != SUCCESS )
        return ERROR;

    if ( BMP085_GetUP ( ossMode ) != SUCCESS )
        return ERROR;

    BMP085_Data.X1 = BMP085_Data.UT - BMP085_Coefficients.AC6 ;
    BMP085_Data.X1 = ( BMP085_Data.X1 * BMP085_Coefficients.AC5 ) >> 15;
    BMP085_Data.X2 = ( BMP085_Coefficients.MC * pow ( 2, 11 ) ) / ( BMP085_Data.X1 + BMP085_Coefficients.MD );
    BMP085_Data.B5 = ( BMP085_Data.X1 + BMP085_Data.X2 );
    BMP085_Data.Temp = ( ( BMP085_Data.B5 + 8 ) / pow ( 2, 4 ) );

    BMP085_Data.B6 = BMP085_Data.B5 - 4000;
    BMP085_Data.X1 = ( BMP085_Coefficients.B2 * ( BMP085_Data.B6 * BMP085_Data.B6 / pow ( 2, 12 ) ) ) / pow ( 2, 11 );
    BMP085_Data.X2 = BMP085_Coefficients.AC2 * BMP085_Data.B6 / pow ( 2, 11 );
    BMP085_Data.X3 = BMP085_Data.X1 + BMP085_Data.X2;
    BMP085_Data.B3 = ( ( BMP085_Coefficients.AC1 * 4 + BMP085_Data.X3 ) << ossMode + 2 ) / 4;
    BMP085_Data.X1 = BMP085_Coefficients.AC3 * BMP085_Data.B6 / pow ( 2, 13 );
    BMP085_Data.X2 = ( BMP085_Coefficients.B1 * ( BMP085_Data.B6 * BMP085_Data.B6 / pow ( 2, 12 ) ) ) / pow ( 2, 16 );
    BMP085_Data.X3 = ( ( BMP085_Data.X1 + BMP085_Data.X2 ) + 2 ) / pow ( 2, 2 );
    BMP085_Data.B4 = BMP085_Coefficients.AC4 * ( uint32_t ) ( BMP085_Data.X3 + 32768 ) / pow ( 2, 15 );
    BMP085_Data.B7 = ( ( uint32_t ) BMP085_Data.UP - BMP085_Data.B3 ) * ( 5000 >> ossMode );

    if ( BMP085_Data.B7 < 0x80000000 )
    {
        BMP085_Data.Pressure = ( BMP085_Data.B7 * 2 ) / BMP085_Data.B4;
    }

    else
    {
        BMP085_Data.Pressure = ( BMP085_Data.B7 / BMP085_Data.B4 ) * 2;
    }

    BMP085_Data.X1 = ( BMP085_Data.Pressure / pow ( 2, 8 ) * BMP085_Data.Pressure / pow ( 2, 8 ) );
    BMP085_Data.X1 = ( BMP085_Data.X1 * 3038 ) / pow ( 2, 16 );
    BMP085_Data.X2 = ( -7357 * BMP085_Data.Pressure ) / pow ( 2, 16 );
    BMP085_Data.Pressure = BMP085_Data.Pressure + ( BMP085_Data.X1 + BMP085_Data.X2 + 3791 ) / pow ( 2, 4 );

    //Everything OK
    return SUCCESS;
}


bool BMP085_GetUT()
{
    //Send Command to measure
    if ( TWI_MasterWrite ( &twiMaster, BMP085_Adress, &BMP085_CtrlRegTemp[0], 2 ) == SUCCESS )
    {
        while ( twiMaster.status != TWIM_STATUS_READY );

        _delay_ms ( BMP085_WaitTimeMeasureUT );

        //Read UT
        if ( TWI_MasterWriteRead ( &twiMaster, BMP085_Adress, &BMP085_StartAdressTemp, 1, 2 ) == SUCCESS )
        {
            while ( twiMaster.status != TWIM_STATUS_READY );

            BMP085_Data.UT = twiMaster.readData[0] << 8 | twiMaster.readData[1];
            return SUCCESS;
        }

        else return ERROR;

    }

    else return ERROR;


    return SUCCESS;
}


bool BMP085_GetUP ( uint8_t ossMode )
{
    uint8_t CtrlRegPressure[2];
    double waitTime;

    //Evaluate Register and wait time depend on ossMode
    switch ( ossMode )
    {
        case 0: //OSRS0 ultra low power
            waitTime = BMP085_WaitTimePressureOSRS0;
            CtrlRegPressure[0] = BMP085_CtrlRegPressureOSS0[0];
            CtrlRegPressure[1] = BMP085_CtrlRegPressureOSS0[1];
            break;

        case 1: //OSRS1 standard
            waitTime = BMP085_WaitTimePressureOSRS1;
            CtrlRegPressure[0] = BMP085_CtrlRegPressureOSS1[0];
            CtrlRegPressure[1] = BMP085_CtrlRegPressureOSS1[1];
            break;

        case 2: //OSRS2 high resolution
            waitTime = BMP085_WaitTimePressureOSRS2;
            CtrlRegPressure[0] = BMP085_CtrlRegPressureOSS2[0];
            CtrlRegPressure[1] = BMP085_CtrlRegPressureOSS2[1];
            break;

        case 3: //OSRS3 ultra high resolution
            waitTime = BMP085_WaitTimePressureOSRS3;
            CtrlRegPressure[0] = BMP085_CtrlRegPressureOSS3[0];
            CtrlRegPressure[1] = BMP085_CtrlRegPressureOSS3[1];
            break;
    }

    //Send Command to measure
    if ( TWI_MasterWrite ( &twiMaster, BMP085_Adress, CtrlRegPressure, 2 ) == SUCCESS )
    {
        while ( twiMaster.status != TWIM_STATUS_READY );

        _delay_ms ( BMP085_WaitTimePressureOSRS3 );

        //Read UT
        if ( TWI_MasterWriteRead ( &twiMaster, BMP085_Adress, &BMP085_StartAdressPressure, 1, 3 ) == SUCCESS )
        {
            while ( twiMaster.status != TWIM_STATUS_READY );

            BMP085_Data.UP = ( twiMaster.readData[0] << 16 | twiMaster.readData[1] << 8 | twiMaster.readData[2] ) >> ( 8 - ossMode );
            return SUCCESS;
        }

        else return ERROR;

    }

    else return ERROR;


    return SUCCESS;
}