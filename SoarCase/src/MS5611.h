#ifndef MS5611_H_
#define MS5611_H_


#include "math.h"
#include "Globals.h"
#include "BoardInit.h"
#include "TWI.h"


#define VARIOBUFFERLENGTH 20				//Buffer for moving average
#define TIMEBETEWEENMEASUREMENTS 0.019335	//Time in s between measurements
#define HEIGHTFILTERFACTOR 2				//Filter for averaging height



enum MS5611_Status
{
    MS5611_STATUS_IDLE,
    MS5611_STATUS_ERROR,
    MS5611_STATUS_UNINIT,
    MS5611_STATUS_WAITD1,
    MS5611_STATUS_WAITD2,
    MS5611_STATUS_D1OK,
    MS5611_STATUS_D2OK,
};

enum MS5611_OSRMode
{
    MS5611_OSR256,
    MS5611_OSR512,
    MS5611_OSR1024,
    MS5611_OSR2048,
    MS5611_OSR4096
};

struct MS5611_Data_struct
{
    int32_t pressure_raw;				//Pressure in dPa (0.001mbar) without filtering
    int32_t pressure_filtered;			//Pressure in dPa with noise reduction
    int32_t temperature;				//Temperature 0.01 resolution
    uint16_t c[8];						//Calibration data
    uint32_t d1;						//Digital pressure value
    uint32_t d2;						//Digital temperature value
    uint32_t sum;						//Sum for filtering
    uint8_t filterFactor;				//Factor for the noise reduction
    uint32_t altitude;					//Height in centimeters
    uint32_t altitudeOld;				//Height in centimeters from prev. calculation
    int32_t vario;						//Actual vario value in cm/s
    int32_t varioBuffer[VARIOBUFFERLENGTH];		//Buffer of vario values to calc. moving average in cm/s
    uint8_t varioStoreWrite;			//Position of the act. value in buffer
    int32_t varioSum;					//Sum of all vario values in the buffer in cm/ms*buffersize
};

struct MS5611_struct
{
    uint8_t adress;
    uint8_t OSR_Mode;
    enum MS5611_Status status;
    bool initialized;
    struct MS5611_Data_struct data;
    bool NewData;
    float pressureCorrection;
};

struct MS5611_Data_struct MS5611_Data;
struct MS5611_struct MS5611;



extern bool MS5611_Init ( uint8_t OSR_Mode );
extern bool MS5611_GetData ( void );
extern void MS5611_SetQNH ( double qnh );
extern uint32_t MS5611_GetVS ( void );


bool MS5611_DataInit ( uint8_t OSR_Mode );
bool MS5611_CalcData ( void );
double MS5611_CalcHeight ( int64_t pressure );
void MS5611_CalcVario ( void );
void MS5611_PressureAverage ( void );


#endif /* MS5611_H_ */