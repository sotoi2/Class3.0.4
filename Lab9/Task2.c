#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/fpu.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"

#define TARGET_IS_BLIZZARD_RB1
// used for assigning radians value
#ifndef M_PI
#define M_PI                    3.14159265358979323846
#endif

#define SERIES_LENGTH 100

float gSeriesData[SERIES_LENGTH];

int32_t i32DataCount = 0;

int main(void)
{
    float fRadians;
    //enable fpu calculations
    ROM_FPULazyStackingEnable();
    ROM_FPUEnable();
    // set clock
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
    // set value for radians

    fRadians = ((2 * M_PI) / SERIES_LENGTH);
    // count for 100 times
    while(i32DataCount < SERIES_LENGTH)
    {
        // equation to graph / create
        gSeriesData[i32DataCount] = sinf( fRadians * i32DataCount + 50*i32DataCount) + 0.5*cosf(fRadians * i32DataCount + 200*i32DataCount);

        i32DataCount++;
    }

    while(1)
    {
    }
}
