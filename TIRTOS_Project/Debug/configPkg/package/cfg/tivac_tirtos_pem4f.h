/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-B24
 */

#include <xdc/std.h>

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle adcsem;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle ADCTask;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle uartsem;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle uarttask;

#include <ti/sysbios/hal/Hwi.h>
extern const ti_sysbios_hal_Hwi_Handle Timer2A;

#include <ti/sysbios/knl/Task.h>
extern const ti_sysbios_knl_Task_Handle readsw;

#include <ti/sysbios/knl/Semaphore.h>
extern const ti_sysbios_knl_Semaphore_Handle readsem;

extern int xdc_runtime_Startup__EXECFXN__C;

extern int xdc_runtime_Startup__RESETFXN__C;

