#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
#include "driverlib/i2c.h"
#include "sensorlib/i2cm_drv.h"
#include "driverlib/pin_map.h"
#include "inc/hw_memmap.h"
#include "sensorlib/hw_mpu6050.h"
#include "sensorlib/mpu6050.h"
#include "inc/hw_ints.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_i2c.h"
#include "inc/hw_types.h"
#include "driverlib/rom_map.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include <string.h>
#include "utils/uartstdio.h"



volatile bool g_bMPU6050Done;


    tI2CMInstance g_sI2CMSimpleInst;

    void I2CMSimpleIntHandler(void)
    {

        I2CMIntHandler(&g_sI2CMSimpleInst);
    }


    void InitI2C0(void)
{
        //enable I2C module 0
        SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);

        //reset module
        SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);

        //enable GPIO peripheral that contains I2C 0
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

        // Configure the pin muxing for I2C0 functions on port B2 and B3.
        GPIOPinConfigure(GPIO_PB2_I2C0SCL);
        GPIOPinConfigure(GPIO_PB3_I2C0SDA);

        // Select the I2C function for these pins.
        GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
        GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

        // Enable and initialize the I2C0 master module.  Use the system clock for
        // the I2C0 module.
        // I2C data transfer rate set to 400kbps.
        I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), true);

        //clear I2C FIFOs
        HWREG(I2C0_BASE + I2C_O_FIFOCTL) = 80008000;

        // Initialize the I2C master driver.
        I2CMInit(&g_sI2CMSimpleInst, I2C0_BASE, INT_I2C0, 0xff, 0xff, SysCtlClockGet());

}

void MPU6050Callback(void *pvCallbackData, uint_fast8_t ui8Status)
{

    if (ui8Status != I2CM_STATUS_SUCCESS)
    {

    }

    g_bMPU6050Done = true;
}




//
// The MPU6050 example.
//
void MPU6050Example(void)
{
    float fAccel[3], fGyro[3];
    float gx, gy, gz;
    float ax, ay, az;

    tMPU6050 sMPU6050;

    //
    // Initialize the MPU6050. This code assumes that the I2C master instance
    // has already been initialized.
    //
    g_bMPU6050Done = false;
    MPU6050Init(&sMPU6050, &g_sI2CMSimpleInst, 0x68, MPU6050Callback, &sMPU6050);
    while (!g_bMPU6050Done)
    {
    }
    //
    // Configure the MPU6050 for +/- 4 g accelerometer range.
    //
    g_bMPU6050Done = false;
    MPU6050ReadModifyWrite(&sMPU6050, MPU6050_O_ACCEL_CONFIG,
        ~MPU6050_ACCEL_CONFIG_AFS_SEL_M,
        MPU6050_ACCEL_CONFIG_AFS_SEL_4G, MPU6050Callback, &sMPU6050);
    while (!g_bMPU6050Done)
    {
    }



    g_bMPU6050Done = false;
    MPU6050ReadModifyWrite(&sMPU6050, MPU6050_O_PWR_MGMT_1, 0x00, 0b00000010 & MPU6050_PWR_MGMT_1_DEVICE_RESET, MPU6050Callback, &sMPU6050);
    while (!g_bMPU6050Done)
    {
    }

    g_bMPU6050Done = false;
    MPU6050ReadModifyWrite(&sMPU6050, MPU6050_O_PWR_MGMT_2, 0x00, 0x00, MPU6050Callback, &sMPU6050);
    while (!g_bMPU6050Done)
    {
    }






    //
    // Loop forever reading data from the MPU6050. Typically, this process
    // would be done in the background, but for the purposes of this example,
    // it is shown in an infinite loop.
    //
    while (1)
    {
        //
        // Request another reading from the MPU6050.
        //
        g_bMPU6050Done = false;
        MPU6050DataRead(&sMPU6050, MPU6050Callback, &sMPU6050);
        while (!g_bMPU6050Done)
        {


        }
        //
        // Get the new accelerometer and gyroscope readings.
        //
        MPU6050DataAccelGetFloat(&sMPU6050, &fAccel[0], &fAccel[1],
            &fAccel[2]);
        MPU6050DataGyroGetFloat(&sMPU6050, &fGyro[0], &fGyro[1], &fGyro[2]);
        //
        // Do something with the new accelerometer and gyroscope readings.
        //


        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

            GPIOPinConfigure(GPIO_PA0_U0RX);
            GPIOPinConfigure(GPIO_PA1_U0TX);

            GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
            UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

            UARTStdioConfig(0, 115200, 16000000);
            //These variables were used because the original values were too small
            //to read easily.



            ax = fAccel[0] * 15000;
            ay = fAccel[1] * 15000;
            az = fAccel[2] * 15000;

            gx = fGyro[0] * 15000;
            gy = fGyro[1] * 15000;
            gz = fGyro[2] * 15000;

            // This is where the UART will be configured and enabled.
            // the clock is enabled to the gpioa and the uart
                SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
                SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
                GPIOPinConfigure(GPIO_PA0_U0RX);
                GPIOPinConfigure(GPIO_PA1_U0TX);
                GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
                UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
                UARTStdioConfig(0, 115200, 16000000);



                // printing the necessary values

                           UARTprintf("Accelerometer X: %d", (int)ax);

                           UARTprintf(" Acceloerometer Y: %d", (int)ay);

                           UARTprintf(" Accelerometer Z: %d\n", (int)az);





                            UARTprintf("Gyro X: %d", (int)gx);

                            UARTprintf(" Gyro Y: %d", (int)gy);

                            UARTprintf(" Gyro Z: %d\n", (int)gz);



// a delay to make the data easier to aread
            SysCtlDelay(2500000);

    }
}

int main()
{
    //clock setting
    SysCtlClockSet(SYSCTL_SYSDIV_1|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    //call the
    InitI2C0();
    MPU6050Example();
    return(0);
}
