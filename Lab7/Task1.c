    #include <stdint.h>
    #include <stdbool.h>
    #include "inc/hw_memmap.h"
    #include "inc/hw_types.h"
    #include "driverlib/gpio.h"
    #include "driverlib/pin_map.h"
    #include "driverlib/sysctl.h"
    #include "driverlib/uart.h"
    #include "driverlib/interrupt.h"
    #include "driverlib/adc.h"
    #include "driverlib/rom.h"
    #include "inc/hw_ints.h"
    #include "driverlib/timer.h"


volatile uint32_t ui32Period;
    volatile uint32_t ui32TempAvg;
    volatile uint32_t ui32TempValueC;
    volatile uint32_t ui32TempValueF;

    volatile  int i;


    void Timer1IntHandler(void)
{

        uint8_t characters[10];
uint32_t ui32ADC0[4];


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
           ROM_ADCSequenceDataGet(ADC0_BASE, 2, ui32ADC0);

           // calculate the average of the temperature sensor data
           ui32TempAvg = (ui32ADC0[0] + ui32ADC0[1] + ui32ADC0[2] + ui32ADC0[3] + 2)/4;
           // calculate celsius value
           ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;
           // calculate farenheit value
           ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;

int count = 5;
         i = 0;

         while(ui32TempValueF != 0)
         {
         characters[i++] = (ui32TempValueF%10)+ '0';
         ui32TempValueF /=10;
         }

         for( i = 0; i<count; i++)
         {
         UARTCharPut(UART0_BASE, characters[i]);
         }
         UARTCharPut(UART0_BASE, 'F');
         UARTCharPut(UART0_BASE, '\n');
         UARTCharPut(UART0_BASE, '\r');


}






int main(void) {


     SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
     SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
     SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
     GPIOPinConfigure(GPIO_PA0_U0RX);
     GPIOPinConfigure(GPIO_PA1_U0TX);

     GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

     UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
     (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));








     ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
     ROM_ADCHardwareOversampleConfigure(ADC0_BASE, 64);

     ROM_ADCSequenceConfigure(ADC0_BASE, 2, ADC_TRIGGER_PROCESSOR, 0);
     ROM_ADCSequenceStepConfigure(ADC0_BASE, 2, 0, ADC_CTL_TS);
     ROM_ADCSequenceStepConfigure(ADC0_BASE, 2, 1, ADC_CTL_TS);
     ROM_ADCSequenceStepConfigure(ADC0_BASE, 2, 2, ADC_CTL_TS);

     ROM_ADCSequenceStepConfigure(ADC0_BASE,2,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);



     ROM_ADCSequenceEnable(ADC0_BASE, 2);





ADCIntEnable(ADC0_BASE,2);
 //     UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
ROM_SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);                // enable GPIO peripherals
       GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);   // configure pins as outputs for LEDs


       SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);   // enable clock to timer1
           TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);     // configure timer 1 in periodic mode


               ui32Period = (SysCtlClockGet() / 1) / 2;     // sets the delay
               TimerLoadSet(TIMER1_BASE, TIMER_A, ui32Period -1);   // load into Timer's Interval Load register

               IntEnable(INT_TIMER1A);  // enables specific vector associated with Timer 0A
               TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT); // enables a specific event within the timer to generate an interrupt (on timeouts)
              // IntMasterEnable();  // master interrupt enable for all interrupts

               TimerEnable(TIMER1_BASE, TIMER_A);// finally enable the timer



     while (1)
     {

     }



}
