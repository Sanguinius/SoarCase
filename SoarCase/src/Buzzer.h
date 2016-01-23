#ifndef BUZZER_H_
#define BUZZER_H_


#include <avr/io.h>
#include "Globals.h"
#include "BoardInit.h"
#include "MS5611.h"

#define PEEPTIMERFREQ	31250	//Counts per 1 sec
#define PEEPMINPERMSEC	1		//Peep 1 s
#define PEEPMAXPERMSEC	15		//Peep 1 s

#define PWMBUZZERCKL 5000000 //4000000

#define VARIOGROUNDFREQENCY 5400
#define VARIODIFFPER_MMPERSEC 1

//Tonefrequencys in cHz 1Hz->100dHz
#define B5 9878
#define A5 8800
#define G5 7840
#define F5 6985
#define E5 6593
#define D5 5873
#define C5 5232
#define B4 4938
#define A4 4400
#define G4 3919
#define F4 3492
#define E4 3296
#define D4 2937
#define C4 2616

uint32_t peepCountMax;
uint32_t peepCountMin;
uint32_t peepCountRange;

enum Buzzer_Status {
    BUZZER_STATUS_NOTINITIALIZED,
    BUZZER_STATUS_INITIALIZED,
    BUZZER_STATUS_ERROR,
    BUZZER_STATUS_VARIOMODE,
    BUZZER_STATUS_MELODYMODE,
};

enum Buzzer_Loudness {
    BUZZER_VOLUME_MUTE,
    BUZZER_VOLUME_NOTMUTE,
};

struct Buzzer_Struct {
    uint8_t State;
    uint8_t Volume;
    uint64_t Frequency;
    uint64_t DutyCycle;
    bool NeedUpdate;
    uint8_t DutyCyclePercent;
    uint16_t *Melody;
    uint8_t MelodyToneCounter;
    uint8_t MelodyLengthCounter;
} Buzzer;


static const uint16_t melodyBeep1[][2] = {
    {1, 500}, //Dataength + Speed for counter
    {E4, 4},
};

static const uint16_t melodyBeep2[][2] = {
	{1, 500}, //Dataength + Speed for counter
	{G4, 4},
};

static const uint16_t melodyBeep3[][2] = {
	{1, 500}, //Dataength + Speed for counter
	{B4, 4},
};

static const uint16_t melodyBeep4[][2] = {
	{1, 500}, //Dataength + Speed for counter
	{D5, 4},
};

static const uint16_t melodyBeep5[][2] = {
	{1, 500}, //Dataength + Speed for counter
	{F5, 4},
};

static const uint16_t melodyWarning[][2] = {
    {11, 1000}, //Dataength + Speed for counter
    {F4, 4},
    {G4, 1},
    {A4, 4},
    {B4, 1},
    {C5, 4},
    {0, 8},
    {D5, 4},
    {C5, 1},
    {B4, 4},
    {A4, 1},
    {G4, 4},
};

static const uint16_t melodyAirspace[][2] = {
    {7, 2000}, //Dataength + Speed for counter
    {8000, 1},
    {0, 1},
    {7000, 1},
    {0, 4},
    {8000, 1},
    {0, 1},
    {7000, 1},
};

static const uint16_t melodyStartup[][2] = {
    {6, 300}, //Dataength + Speed for counter
    {F4, 4},
    {G4, 4},
    {A4, 4},
    {B4, 4},
    {C5, 4},
    {D5, 4},
};

static const uint16_t melodyFlarmIn[][2] = {
    {2, 2000}, //Dataength + Speed for counter
    {G4, 4},
    {D5, 1},
};

static const uint16_t melodyFlarmOut[][2] = {
    {2, 2000}, //Dataength + Speed for counter
    {D5, 4},
    {G4, 1},
};

static const uint16_t melodyMessage[][2] = {
    {3, 1000}, //Dataength + Speed for counter
    {G4, 2},
    {0 , 2},
    {G4, 5},
};

static const uint16_t melodyTask[][2] = {
	{6, 1000}, //Dataength + Speed for counter
	{G4, 2},
	{0 , 1},
	{G4, 2},
	{0, 5},
	{A4, 5},
	{G4, 5},
};

extern void Buzzer_Init ( void );
extern void Buzzer_Update ( void );

void Buzzer_SetDuty ( int8_t duty );
void Buzzer_SetFreq ( int16_t freq );
void Buzzer_SetBeepFreq ( int8_t dutyCycle );

void Buzzer_VarioSound ( int32_t mm_s );

void Buzzer_PlayMelody ( uint16_t const melody[][2] );

#endif /* BUZZER_H_ */