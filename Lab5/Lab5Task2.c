#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/interrupt.h"
// Rather than adding the peripheral driver library, we call them from rom. less code size
#define TARGET_IS_BLIZZARD_RB1
#include "driverlib/rom.h"


    //aray storing data read from ADC fifo
    uint32_t ui32ADC0Value[4];


    // variables for calculating temp from sensor data
    volatile uint32_t ui32TempAvg;
    volatile uint32_t ui32TempValueC;
    volatile uint32_t ui32TempValueF;
    volatile uint32_t ui32Period;


void Timer1IntHandler(void)
{
    TimerIntClear(TIMER1_BASE,TIMER_A);// Always clear the interrupt for the values that may depend on it in the future

    // Clear the ADC interrup status flag
             ROM_ADCIntClear(ADC0_BASE, 2);
            // Trigger ADC conversion with software
            ROM_ADCProcessorTrigger(ADC0_BASE, 2);


           // waith for the conversion to complete
           while(!ROM_ADCIntStatus(ADC0_BASE, 2, false))
           {
           }



           // we can read the ADC value from the ADC sample sequencer 1 FIFO
           ROM_ADCSequenceDataGet(ADC0_BASE, 2, ui32ADC0Value);

           // calculate the average of the temperature sensor data
           ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
           // calculate celsius value
           ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;
           // calculate farenheit value
           ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;



    if(ui32TempValueF > 72)
    {

        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 2);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);   // turn on the led when the temperature is grreater than 72
    }
    else
        if(ui32TempValueF <= 72 )
        {


            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);   //turn off the led if the temperature goes below 72
        }

}

int main(void)
{


    // set clock to 40 MHz
       ROM_SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);                // enable GPIO peripherals
       GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);   // configure pins as outputs for LEDs


       SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);   // enable clock to timer1
           TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);     // configure timer 1 in periodic mode

          // SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);    // enable clock to peripherals
               //TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);   // Configure Timer 0 in periodic mode

               ui32Period = (SysCtlClockGet() / 1) / 2;     // sets the delay
               TimerLoadSet(TIMER1_BASE, TIMER_A, ui32Period -1);   // load into Timer's Interval Load register

               IntEnable(INT_TIMER1A);  // enables specific vector associated with Timer 0A
               TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT); // enables a specific event within the timer to generate an interrupt (on timeouts)
               IntMasterEnable();  // master interrupt enable for all interrupts

               TimerEnable(TIMER1_BASE, TIMER_A);// finally enable the timer



    // ENABLE THE ADC0 Peripheral
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);


    // number of samples to be averaged  32 for task 2
    ROM_ADCHardwareOversampleConfigure(ADC0_BASE, 32);




    //configure the ADC Sequencer ( ADC0, sample sequencer 1, processor triggers the sequence, highest priority)
    ROM_ADCSequenceConfigure(ADC0_BASE, 2, ADC_TRIGGER_PROCESSOR, 0);


    // configure the four steps in the sequencer, 0-2 on sequencer 1 to sample temp (ADC_CTL_TS), ADC0, sequencer 1, step 0-2...
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 2, 0, ADC_CTL_TS);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 2, 1, ADC_CTL_TS);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 2, 2, ADC_CTL_TS);





    // The last must sample the temp and configure the interrupt flag to be set when sample is done. Tell ADC logic that this is the last conversion on seq 1
    ROM_ADCSequenceStepConfigure(ADC0_BASE,2,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);

    // Enable the Sequencer 1 adc
    ROM_ADCSequenceEnable(ADC0_BASE, 2);
    ADCIntEnable(ADC0_BASE,2);
    while(1)
    {


    }
}

