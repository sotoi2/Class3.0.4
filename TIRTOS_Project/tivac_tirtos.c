#include <xdc/std.h>
#include <ti/sysbios/BIOS.h>
#include <xdc/runtime/Log.h>
#include <xdc/cfg/global.h>
#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/adc.h"
#include "utils/uartstdio.h"
#include "utils/uartstdio.c"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"

#define PWM_FREQUENCY 55


void hwinitialize(void);
void initializec(void);
void displayuart(void);
void Timer_ISR(void);
void ADCinitialize();
void ADCread(void);




volatile int16_t i16TCount = 0;
volatile int16_t i16ICount = 0;


volatile int16_t counter = 30;

uint32_t aveADC;


uint32_t ADCValues[4];

void main(void)
{

   hwinitialize();
   ADCinitialize();
   initializec();

   BIOS_start();

}



void hwinitialize(void)
{
	uint32_t ui32Period;
	uint32_t ui32Load;
    uint32_t ui32PWMClock;




	//set clock
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

    // set the appropriate pins for output
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
    //pwm initialization
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    ui32PWMClock = SysCtlClockGet() / 64;
    ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;
    GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0); //PD0 PWM pin
    GPIOPinConfigure(GPIO_PD0_M1PWM0);
    PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, ui32Load);
    PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, true);
    PWMGenEnable(PWM1_BASE, PWM_GEN_0);




	// LED lighting
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 4);

	// pushbutton config
	GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_4, GPIO_DIR_MODE_IN);

	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	// Timer2 code clock, mode, and setiting the period, along with the interrupt enable
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
	TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC);
	ui32Period = (SysCtlClockGet() / 500);
	TimerLoadSet(TIMER2_BASE, TIMER_A, ui32Period);
	TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT;
	TimerEnable(TIMER2_BASE, TIMER_A);

}


void initializec(void)
{
    //enable the clock to the peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0); // enabling uart

    //makek the clcok for uart
      UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
      GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    //configure the rx, tx pin
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);




    UARTStdioConfig(0, 115200, 16000000);
}

// Initializes ADC1
void ADCinitialize() {
   // enable the clock to the adc, and gpio enable for e
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
   // configur pe3
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);


   // configure sequence and base of adc like previous labs.
    ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);

    ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH3);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH3);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH3);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 3, ADC_CTL_CH3 | ADC_CTL_IE | ADC_CTL_END);

    // sequence enabler
    ADCSequenceEnable(ADC0_BASE, 1);


}




void Timer_ISR(void)
{
    TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);

   // if statement to write to led
    if (GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_0))
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 4);
    }

    else{
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);

    }


    if(i16ICount == 10) {
        Semaphore_post(adcsem);
    }

    else if (i16ICount == 20) {
        Semaphore_post(uartsem);
    }

    else if(i16ICount == 30) {
        Semaphore_post(readsem);

        i16ICount = 0;
    }

    i16ICount++;
}


void readsw(void)
{
    while(1)
    {
        if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)==0x00) {
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, aveADC);
        }

        Semaphore_pend(readsem, BIOS_WAIT_FOREVER);
    }
}


void ADCread(void) {

    while(1) {
        ADCIntClear(ADC0_BASE, 1);

        ADCProcessorTrigger(ADC0_BASE, 1);


        while(!ADCIntStatus(ADC0_BASE, 1, false))
        {
        }

        //read the adc
        ADCSequenceDataGet(ADC0_BASE, 1, ADCValues);
        aveADC = (ADCValues[0] + ADCValues[1] + ADCValues[2] + ADCValues[3])/4;
        Semaphore_pend(adcsem, BIOS_WAIT_FOREVER);
    }

}


void displayuart(void)
{
    while(1)
    {
        UARTprintf("ADC1 Channel3: %d\n", aveADC);
        Semaphore_pend(uartsem, BIOS_WAIT_FOREVER);
    }
}
