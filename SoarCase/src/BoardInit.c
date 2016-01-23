#include "BoardInit.h"

GPIO_t LED_Green = {&PORTE, PIN4_bm};
GPIO_t LED_Red = {&PORTE, PIN1_bm};
GPIO_t LED_Blue = {&PORTE, PIN0_bm};

GPIO_t BT_Connected = {&PORTF, PIN0_bm};

GPIO_t S1 = {&PORTD, PIN1_bm};
GPIO_t S2 = {&PORTA, PIN6_bm};
GPIO_t S3 = {&PORTA, PIN5_bm};
GPIO_t S4 = {&PORTA, PIN4_bm};
GPIO_t S5 = {&PORTA, PIN7_bm};
GPIO_t S6 = {&PORTB, PIN6_bm};
GPIO_t S7S = {&PORTF, PIN7_bm};
GPIO_t S7AI = {&PORTF, PIN5_bm};
GPIO_t S7BI = {&PORTF, PIN6_bm};
GPIO_t S7AO = {&PORTA, PIN0_bm};
GPIO_t S7BO = {&PORTA, PIN1_bm};
GPIO_t S8S = {&PORTB, PIN5_bm};
GPIO_t S8AI = {&PORTB, PIN7_bm};
GPIO_t S8BI = {&PORTB, PIN4_bm};
GPIO_t S8AO = {&PORTB, PIN3_bm};
GPIO_t S8BO = {&PORTB, PIN2_bm};

void BoardInit()
{
    //Int_SysClkInit();
    //Ext_SysClkInit();
    LUFA_SysClkInit();
    
    GPIOInit();
    
    TWIInit();
    
    CounterInit();
    
    ActivateISR();
}



void Int_SysClkInit()
{
    CCP = CCP_IOREG_gc;              // disable register security for oscillator update
    OSC.CTRL = OSC_RC32MEN_bm;       // enable 32MHz oscillator
    
    while ( ! ( OSC.STATUS & OSC_RC32MRDY_bm ) ); // wait for oscillator to be ready
    
    CCP = CCP_IOREG_gc;              // disable register security for clock update
    CLK.CTRL = CLK_SCLKSEL_RC32M_gc; // switch to 32MHz clock
}


void Ext_SysClkInit()
{
    OSC.XOSCCTRL = OSC_FRQRANGE_12TO16_gc | OSC_XOSCSEL_XTAL_256CLK_gc; /* configure the XTAL input */
    OSC.CTRL |= OSC_XOSCEN_bm; /* start XTAL */
    
    while ( ! ( OSC.STATUS & OSC_XOSCRDY_bm ) ); /* wait until ready */
    
    OSC.PLLCTRL = OSC_PLLSRC_XOSC_gc | 0x2; /* XTAL->PLL, 4x multiplier */
    OSC.CTRL |= OSC_PLLEN_bm; /* start PLL */
    
    while ( ! ( OSC.STATUS & OSC_PLLRDY_bm ) ); /* wait until ready */
    
    CCP = CCP_IOREG_gc; /* allow changing CLK.CTRL */
    CLK.CTRL = CLK_SCLKSEL_PLL_gc; /* use PLL output as system clock */
}

#ifdef LUFA
void LUFA_SysClkInit()
{
    // Start the PLL to multiply the 2MHz RC oscillator to F_CPU and switch the CPU core to run from it
    XMEGACLK_StartPLL ( CLOCK_SRC_INT_RC2MHZ, 2000000, F_CPU );
    XMEGACLK_SetCPUClockSource ( CLOCK_SRC_PLL );
    
    // Start the 32MHz internal RC oscillator and start the DFLL to increase it to F_USB using the USB SOF as a reference
    XMEGACLK_StartInternalOscillator ( CLOCK_SRC_INT_RC32MHZ );
    XMEGACLK_StartDFLL ( CLOCK_SRC_INT_RC32MHZ, DFLL_REF_INT_USBSOF, F_USB );
}
#endif


void GPIOInit ( void )
{
    //Deactivate JTAG
    CCP = CCP_IOREG_gc; //Allow protected IO changing
    MCU_MCUCR = 0x01;
    
    //LED
    LED_Green.GPIO_Port->DIRSET = LED_Green.GPIO_Pin;
    LED_Green.GPIO_Port->OUTSET = LED_Green.GPIO_Pin;
    LED_Red.GPIO_Port->DIRSET = LED_Red.GPIO_Pin;
    LED_Red.GPIO_Port->OUTSET = LED_Red.GPIO_Pin;
    LED_Blue.GPIO_Port->DIRSET = LED_Blue.GPIO_Pin;
    LED_Blue.GPIO_Port->OUTSET = LED_Blue.GPIO_Pin;
    
    //BT Connected Input
    BT_Connected.GPIO_Port->DIRCLR = BT_Connected.GPIO_Pin;
    BT_Connected.GPIO_Port->PIN0CTRL = PORT_OPC_PULLDOWN_gc;
    
    //Push-buttons
    S1.GPIO_Port->DIRCLR = S1.GPIO_Pin;
    S1.GPIO_Port->PIN1CTRL = PORT_OPC_PULLUP_gc;
    S2.GPIO_Port->DIRCLR = S2.GPIO_Pin;
    S2.GPIO_Port->PIN6CTRL = PORT_OPC_PULLUP_gc;
    S3.GPIO_Port->DIRCLR = S3.GPIO_Pin;
    S3.GPIO_Port->PIN5CTRL = PORT_OPC_PULLUP_gc;
    S4.GPIO_Port->DIRCLR = S4.GPIO_Pin;
    S4.GPIO_Port->PIN4CTRL = PORT_OPC_PULLUP_gc;
    S5.GPIO_Port->DIRCLR = S5.GPIO_Pin;
    S5.GPIO_Port->PIN7CTRL = PORT_OPC_PULLUP_gc;
    S6.GPIO_Port->DIRCLR = S6.GPIO_Pin;
    S6.GPIO_Port->PIN6CTRL = PORT_OPC_PULLUP_gc;
    S7S.GPIO_Port->DIRCLR = S7S.GPIO_Pin;
    S7S.GPIO_Port->PIN7CTRL = PORT_OPC_PULLUP_gc;
    S7AI.GPIO_Port->DIRCLR = S7AI.GPIO_Pin;
    S7AI.GPIO_Port->PIN5CTRL = PORT_OPC_PULLUP_gc;
    S7BI.GPIO_Port->DIRCLR = S7BI.GPIO_Pin;
    S7BI.GPIO_Port->PIN6CTRL = PORT_OPC_PULLUP_gc;
    S7AO.GPIO_Port->DIRCLR = S7AO.GPIO_Pin;
    S7AO.GPIO_Port->PIN0CTRL = PORT_OPC_PULLUP_gc;
    S7BO.GPIO_Port->DIRCLR = S7BO.GPIO_Pin;
    S7BO.GPIO_Port->PIN1CTRL = PORT_OPC_PULLUP_gc;
    
    S8S.GPIO_Port->DIRCLR = S8S.GPIO_Pin;
    S8S.GPIO_Port->PIN5CTRL = PORT_OPC_PULLUP_gc;
    S8AI.GPIO_Port->DIRCLR = S8AI.GPIO_Pin;
    S8AI.GPIO_Port->PIN7CTRL = PORT_OPC_PULLUP_gc;
    S8BI.GPIO_Port->DIRCLR = S8BI.GPIO_Pin;
    S8BI.GPIO_Port->PIN4CTRL = PORT_OPC_PULLUP_gc;
    S8AO.GPIO_Port->DIRCLR = S8AO.GPIO_Pin;
    S8AO.GPIO_Port->PIN3CTRL = PORT_OPC_PULLUP_gc;
    S8BO.GPIO_Port->DIRCLR = S8BO.GPIO_Pin;
    S8BO.GPIO_Port->PIN2CTRL = PORT_OPC_PULLUP_gc;
    
    GPIO_t S1 = {&PORTD, PIN1_bm};
    GPIO_t S2 = {&PORTA, PIN6_bm};
    GPIO_t S3 = {&PORTA, PIN5_bm};
    GPIO_t S4 = {&PORTA, PIN4_bm};
    GPIO_t S5 = {&PORTA, PIN7_bm};
    GPIO_t S6 = {&PORTB, PIN6_bm};
    GPIO_t S7S = {&PORTF, PIN7_bm};
    GPIO_t S7AI = {&PORTF, PIN5_bm};
    GPIO_t S7BI = {&PORTF, PIN6_bm};
    GPIO_t S7AO = {&PORTA, PIN0_bm};
    GPIO_t S7BO = {&PORTA, PIN1_bm};
    GPIO_t S8S = {&PORTB, PIN5_bm};
    GPIO_t S8AI = {&PORTB, PIN7_bm};
    GPIO_t S8BI = {&PORTB, PIN4_bm};
    GPIO_t S8AO = {&PORTB, PIN3_bm};
    GPIO_t S8BO = {&PORTB, PIN2_bm};
}



void TWIInit ( void )
{
    /* Initialize TWI master. */
    TWI_MasterInit ( &twiMaster,
                     &TWIC,
                     TWI_MASTER_INTLVL_HI_gc,
                     TWI_BAUDSETTING );
}



void ActivateISR ( void )
{
    PMIC.CTRL |= PMIC_HILVLEN_bm ;
    PMIC.CTRL |= PMIC_MEDLVLEN_bm ;
    PMIC.CTRL |= PMIC_LOLVLEN_bm;
    sei();
}



void SysTickInit ( void )
{
    /* NOT USED NOW
    
    //Activate intern oscillator
    OSC.CTRL	|= OSC_RC32KEN_bm;
    //Use intern oscillator with 1024khz
    //CLK.RTCCTRL	= CLK_RTCSRC_RCOSC_gc | CLK_RTCEN_bm;
    CLK.RTCCTRL	= CLK_RTCSRC_ULP_gc | CLK_RTCEN_bm;
    //Prescaler 1
    RTC.CTRL = RTC_PRESCALER_DIV1_gc;
    
    // Wait until synchron
    while ( RTC.STATUS & RTC_SYNCBUSY_bm );
    
    //Timer overflow Interrupt with Interrupt priority high
    RTC.INTCTRL |= RTC_OVFINTLVL_LO_gc;
    RTC.INTCTRL |= RTC_COMPINTLVL_OFF_gc;
    
    
    //RTC --> ~100Hz
    //Timer top value
    RTC.PER		= 3;
    
    //Set Timer register CNT to 0
    RTC.COMP	= 0;
    
    */
}



void CounterInit ( void )
{
    //Counter C0 for Systemtick (Interrupt 400Hz, 2,5ms)
    TCC0.CTRLA |= TC_CLKSEL_DIV64_gc;		//Gives a count 0.5Mhz
    TCC0.CTRLB |= TC_WGMODE_NORMAL_gc;		//Set timer to normal mode
    TCC0.CNT = 0;							//Set counter to 0
    TCC0.PER = 1280;						//Set top value of the counter (theor. 2500)
    TCC0.INTCTRLA |= TC_OVFINTLVL_MED_gc;	//Set interrupt when overflow (.PER value is reached)
}



void SetOutput ( GPIO_t output )
{
    output.GPIO_Port->OUTCLR = output.GPIO_Pin;
}



void ClearOutput ( GPIO_t output )
{
    output.GPIO_Port->OUTSET = output.GPIO_Pin;
}



void ToggleOutput ( GPIO_t output )
{
    output.GPIO_Port->OUTTGL = output.GPIO_Pin;
}



uint8_t ReadInput ( GPIO_t input )
{
    if ( ! ( input.GPIO_Port->IN & input.GPIO_Pin ) )
        return ON;
        
    else
        return OFF;
}