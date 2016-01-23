#include "Buzzer.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///Buzzer_Init
///////////////////////////////////////////////////////////////////////////////////////////////////
void Buzzer_Init ( void )
{



    //Counter C1 for Buzzer peep frequency (Vario sound generation and melodis)
    TCC1.CTRLA |= TC_CLKSEL_DIV1024_gc;		//Gives a count 31'250Hz
    TCC1.CTRLB |= TC_WGMODE_NORMAL_gc;		//Set timer to normal mode
    TCC1.CNT = 0;							//Set counter to 0
    TCC1.PER = 31250;						//Set top value of the counter
    TCC1.INTCTRLA |= TC_OVFINTLVL_LO_gc;	//Set interrupt when overflow (.PER value is reached)
    
    
    //Counter D0 for Buzzer (PWM-Mode), Buzzer connected on Pin D0
    PORTD.DIR |= PIN0_bm;					//Set D0 as output
    TCD0.CTRLA |= TC_CLKSEL_DIV64_gc;		//Prescaler
    TCD0.CTRLB |= TC_WGMODE_SS_gc;			//Set D0 to single slope PWM
    TCD0.CTRLB |= TC0_CCAEN_bm;				//Enable channel A
    TCD0.CNT = 0;							//Counter 0
    TCD0.PER = 160;							//Top value (period T)
    TCD0.CCA = 0;							//Compare value (duty cycle)
    
    
    Buzzer_SetFreq ( 2500 );
    Buzzer_SetDuty ( 0 );
    Buzzer.State = BUZZER_STATUS_INITIALIZED;
    Buzzer.Volume = BUZZER_VOLUME_MUTE;
    Buzzer.MelodyToneCounter = 0;
    Buzzer.MelodyLengthCounter = 0;
    
    //Calculate Peepsettings
    peepCountMin = PEEPTIMERFREQ / PEEPMINPERMSEC;
    peepCountMax = PEEPTIMERFREQ / PEEPMAXPERMSEC;
    peepCountRange = peepCountMin - peepCountMax;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///Buzzer_Update
///////////////////////////////////////////////////////////////////////////////////////////////////
extern void Buzzer_Update ( void )
{
    if ( Buzzer.NeedUpdate != TRUE )
        return;
        
    if ( Buzzer.State == BUZZER_STATUS_VARIOMODE ) {
        //Buzzer_VarioSound();
    }
    
    if ( Buzzer.State == BUZZER_STATUS_MELODYMODE ) {
        Buzzer_PlayMelody ( Buzzer.Melody );
    }
    
    Buzzer.NeedUpdate = FALSE;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///Buzzer_SetDuty (in percent, 50 => 50%
///////////////////////////////////////////////////////////////////////////////////////////////////
void Buzzer_SetDuty ( int8_t duty )
{
    //Check if input is ok and then set the duty to Compare register A
    if ( duty >= 0 && duty <= 100 ) {
        Buzzer.DutyCyclePercent = duty;
        Buzzer.DutyCycle =  Buzzer.Frequency * duty / 100;
        TCD0.CCA = Buzzer.DutyCycle;
        //TCD0.CNT = 0;
    }
    
    //Check Volume
    if ( Buzzer.DutyCycle > 0 )
        Buzzer.Volume = BUZZER_VOLUME_NOTMUTE;
        
    else
        Buzzer.Volume = BUZZER_VOLUME_MUTE;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///Buzzer_SetFreq (Frequency in dHz)
///////////////////////////////////////////////////////////////////////////////////////////////////
void Buzzer_SetFreq ( int16_t freq )
{
    //Set new frequenzy
    if ( freq > 0 )
        Buzzer.Frequency = ( PWMBUZZERCKL / freq );
        
    else Buzzer.Frequency = 0;
    
    //Set counter settings
    TCD0.PER = Buzzer.Frequency;
    
    if ( TCD0.CNT >= TCD0.PER )
        TCD0.CNT = 0;
        
    Buzzer_SetDuty ( Buzzer.DutyCyclePercent );
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///Buzzer_SetBeepFreq (0=continous tone, 1-100 duty cycle from min to max)
///////////////////////////////////////////////////////////////////////////////////////////////////
void Buzzer_SetBeepFreq ( int8_t dutyCycle )
{
    //Set Timer C1 per if dutyCycle is more than 0
    if ( dutyCycle > 0 )
        TCC1.PER = peepCountMax - peepCountRange * dutyCycle / 100;
        
    //ToD0: Timer off when duty 0, testen funktion
    
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///Buzzer_VarioSound (Generate Vario sound, input vertical speed in cm/s
///////////////////////////////////////////////////////////////////////////////////////////////////
void Buzzer_VarioSound ( int32_t mm_s )
{
    int32_t temp = VARIOGROUNDFREQENCY +  mm_s * VARIODIFFPER_MMPERSEC ;
    
    //Buzzer_SetDuty ( 1 );
    if ( Buzzer.State == BUZZER_STATUS_VARIOMODE )
        Buzzer_SetFreq ( temp );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///Buzzer_PlayMelody
///////////////////////////////////////////////////////////////////////////////////////////////////
void Buzzer_PlayMelody ( uint16_t const melody[][2] )
{
    //First call: Set settings and store state in Buzzer
    if ( Buzzer.MelodyToneCounter == 0 ) {
        //Set speed of melody
        TCC1.CNT = 0;
        TCC1.PER = melody [0][1];
        
        //Set counter to 1, because on 0 there are the melody settings
        Buzzer.MelodyToneCounter = 1;
        
        Buzzer.MelodyLengthCounter = 0;
        
        //Set Buzzer state to Melodymode
        Buzzer.State = BUZZER_STATUS_MELODYMODE;
        
        //Store adress to melody to Buzzer struct
        Buzzer.Melody = melody;
        
        Buzzer_SetDuty ( 4 );
        
        //Play first note
        Buzzer_SetFreq ( melody [Buzzer.MelodyToneCounter][0] );
    }
    
    //Check if a note from previous cycle is still playing
    if ( Buzzer.MelodyLengthCounter >= melody[Buzzer.MelodyToneCounter][1] ) {
        Buzzer.MelodyLengthCounter = 1;
        Buzzer.MelodyToneCounter++;
    }
    
    //Last Tone is still running
    else {
        Buzzer.MelodyLengthCounter++;
        return;
    }
    
    //Check if there are data
    if ( Buzzer.MelodyToneCounter > melody [0][0] ) {
        //Melody finished, return to Vario mode
        Buzzer.State = BUZZER_STATUS_VARIOMODE;
        Buzzer.MelodyLengthCounter = 0;
        Buzzer.MelodyToneCounter = 0;
        Buzzer_SetDuty ( 0 );
        return;
    }
    
    //Play the tone or make a pause
    /*
    if ( melody[Buzzer.MelodyToneCounter][0] == 0 )
    {
        Buzzer_SetBeepFreq ( 0 );
    }
    
    else
    */
    Buzzer_SetFreq ( melody [Buzzer.MelodyToneCounter][0] );
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///TCC1 Interrupt
///////////////////////////////////////////////////////////////////////////////////////////////////
ISR ( TCC1_OVF_vect )
{
    //Buzzer_Update();
    Buzzer.NeedUpdate = TRUE;
}