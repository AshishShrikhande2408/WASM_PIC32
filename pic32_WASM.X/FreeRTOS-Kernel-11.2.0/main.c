/* Kernel includes. */
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "init.h"
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/pthread.h"
#include "UART2.h"


#if defined(__32MZ2048EFM100 )
    #pragma config FMIIEN = OFF, FETHIO = OFF, PGL1WAY = OFF, PMDL1WAY = OFF, IOL1WAY = OFF, FUSBIDIO = OFF
    #pragma config FNOSC = SPLL, FSOSCEN = OFF, IESO = OFF, POSCMOD = EC
    #pragma config OSCIOFNC = OFF, FCKSM = CSECMD, FWDTEN = OFF, FDMTEN = OFF
    #pragma config DMTINTV = WIN_127_128, WDTSPGM = STOP, WINDIS= NORMAL
    #pragma config WDTPS = PS1048576, FWDTWINSZ = WINSZ_25, DMTCNT = DMT31
    #pragma config FPLLIDIV = DIV_3, FPLLRNG = RANGE_13_26_MHZ, FPLLICLK = PLL_POSC
    #pragma config FPLLMULT = MUL_50, FPLLODIV = DIV_2, UPLLFSEL = FREQ_12MHZ
    #pragma config EJTAGBEN = NORMAL, DBGPER = PG_ALL, FSLEEP = OFF, FECCCON = OFF_UNLOCKED
    #pragma config BOOTISA = MIPS32, TRCEN = ON, ICESEL = ICS_PGx2, JTAGEN = OFF, DEBUG = OFF
    #pragma config CP = OFF
#endif /* if defined( __32MZ2048ECM144 ) || defined( __32MZ2048ECH144 ) */

/*-----------------------------------------------------------*/

/* Set mainCREATE_SIMPLE_BLINKY_DEMO_ONLY to one to run the simple blinky demo,
 * or 0 to run the more comprehensive test and demo application. */
#define mainCREATE_SIMPLE_BLINKY_DEMO_ONLY    1
/* Priorities at which the tasks are created. */
#define mainQUEUE_SEND_TASK_PRIORITY       ( tskIDLE_PRIORITY + 1 )
#define mainQUEUE_RECEIVE_TASK_PRIORITY    ( tskIDLE_PRIORITY + 2 )

static pthread_t thread_id_GreenLed,thread_id_AmberLed,thread_id_uart,thread_id_WASM;

void vApplicationMallocFailedHook( void )
{
    /* vApplicationMallocFailedHook() will only be called if
     * configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
     * function that will get called if a call to pvPortMalloc() fails.
     * pvPortMalloc() is called internally by the kernel whenever a task, queue,
     * timer or semaphore is created.  It is also called by various parts of the
     * demo application.  If heap_1.c or heap_2.c are used, then the size of the
     * heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
     * FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
     * to query the size of free heap space that remains (although it does not
     * provide information on how the remaining heap might be fragmented). */
    taskDISABLE_INTERRUPTS();

    for( ; ; )
    {
    }
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
    /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
     * to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
     * task.  It is essential that code added to this hook function never attempts
     * to block in any way (for example, call xQueueReceive() with a block time
     * specified, or call vTaskDelay()).  If the application makes use of the
     * vTaskDelete() API function (as this demo application does) then it is also
     * important that vApplicationIdleHook() is permitted to return to its calling
     * function, because it is the responsibility of the idle task to clean up
     * memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask,
                                    char * pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;

    /* Run time task stack overflow checking is performed if
     * configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook	function is
     * called if a task stack overflow is detected.  Note the system/interrupt
     * stack is not checked. */
    taskDISABLE_INTERRUPTS();

    for( ; ; )
    {
    }
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
    /* This function will be called by each tick interrupt if
    * configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
    * added here, but the tick hook is called from an interrupt context, so
    * code must not attempt to block, and only the interrupt safe FreeRTOS API
    * functions can be used (those that end in FromISR()). */

    #if ( mainCREATE_SIMPLE_BLINKY_DEMO_ONLY == 0 )
    {
        /* Call the periodic queue overwrite from ISR demo. */
        vQueueOverwritePeriodicISRDemo();

        /* Call the queue set ISR test function. */
        vQueueSetAccessQueueSetFromISR();

        /* Exercise event groups from interrupts. */
        vPeriodicEventGroupsProcessing();
    }
    #endif /* if ( mainCREATE_SIMPLE_BLINKY_DEMO_ONLY == 0 ) */
}
/*-----------------------------------------------------------*/

extern void vAssertCalled( const char * pcFile,
                           unsigned long ulLine )
{
    volatile char * pcFileName;
    volatile unsigned long ulLineNumber;

    /* Prevent things that are useful to view in the debugger from being
     * optimised away. */
    pcFileName = ( char * ) pcFile;
    ( void ) pcFileName;
    ulLineNumber = ulLine;

    /* Set ulLineNumber to 0 in the debugger to break out of this loop and
     * return to the line that triggered the assert. */
    while( ulLineNumber != 0 )
    {
        __asm volatile ( "NOP" );
        __asm volatile ( "NOP" );
        __asm volatile ( "NOP" );
        __asm volatile ( "NOP" );
        __asm volatile ( "NOP" );
    }
}

/* This function overrides the normal _weak_ generic handler. */
void _general_exception_handler( void )
{
    static enum
    {
        EXCEP_IRQ = 0,    /* interrupt */
        EXCEP_AdEL = 4,   /* address error exception (load or ifetch) */
        EXCEP_AdES,       /* address error exception (store) */
        EXCEP_IBE,        /* bus error (ifetch) */
        EXCEP_DBE,        /* bus error (load/store) */
        EXCEP_Sys,        /* syscall */
        EXCEP_Bp,         /* breakpoint */
        EXCEP_RI,         /* reserved instruction */
        EXCEP_CpU,        /* coprocessor unusable */
        EXCEP_Overflow,   /* arithmetic overflow */
        EXCEP_Trap,       /* trap (possible divide by zero) */
        EXCEP_FPE = 15,   /* floating point exception */
        EXCEP_IS1 = 16,   /* implementation specific 1 */
        EXCEP_CEU,        /* CorExtend Unuseable */
        EXCEP_C2E,        /* coprocessor 2 */
        EXCEP_DSPDis = 26 /* DSP module disabled */
    }
    _excep_code;

    static unsigned long _epc_code;
    static unsigned long _excep_addr;

    asm volatile ( "mfc0 %0,$13" : "=r" ( _epc_code ) );
    asm volatile ( "mfc0 %0,$14" : "=r" ( _excep_addr ) );

    _excep_code = ( _epc_code & 0x0000007C ) >> 2;

    for( ; ; )
    {
        /* prevent compiler warning */
        ( void ) _excep_code;

        /* Examine _excep_code to identify the type of exception.  Examine
         * _excep_addr to find the address that caused the exception */
        LATESET = 0x0008;
        Nop();
        Nop();
        Nop();
    }
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
    /* Configure the hardware for maximum performance. */
    vHardwareConfigurePerformance();

    /* Setup to use the external interrupt controller. */
    vHardwareUseMultiVectoredInterrupts();

    portDISABLE_INTERRUPTS();

    /* Setup the digital IO for the LED's. */
    vParTestInitialise();
    
    Uart2Initialise(115200UL);
    
    portENABLE_INTERRUPTS();
}
/*-----------------------------------------------------------*/

static void vParTestToggleLED( unsigned portBASE_TYPE uxLED )
{
    unsigned portBASE_TYPE uxLEDBit;

	if( uxLED < 0x100 )
	{
		uxLEDBit = 1 << uxLED;

		/* Use of the LATHINV register removes the need to use a critical 
		section. */
		LATEINV = uxLEDBit;
	}
}

static void *blinkLEDGreen( void * pvParameters )
{
    TickType_t xLastWakeTime;
    /* Remove compiler warnings in the case where configASSERT() is not defined. */
    ( void ) pvParameters;

    const TickType_t xFrequency = 100;
    
    // Initialise the xLastWakeTime variable with the current time.

    xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
        /* This function is called when the blinky software time expires.  All the
        * function does is toggle the LED.  LED mainTIMER_LED should therefore toggle
        * with the period set by mainBLINKY_TIMER_PERIOD. */
       vParTestToggleLED( 4 ); 
       // Wait for the next cycle.
       vTaskDelayUntil( &xLastWakeTime, xFrequency );        
    }
}

static void *blinkLEDAmber( void * pvParameters )
{
    TickType_t xLastWakeTime;
    /* Remove compiler warnings in the case where configASSERT() is not defined. */
    ( void ) pvParameters;

    const TickType_t xFrequency = 500;
    
    // Initialise the xLastWakeTime variable with the current time.

    xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
        /* This function is called when the blinky software time expires.  All the
        * function does is toggle the LED.  LED mainTIMER_LED should therefore toggle
        * with the period set by mainBLINKY_TIMER_PERIOD. */
       vParTestToggleLED( 6 ); 
       // Wait for the next cycle.
       vTaskDelayUntil( &xLastWakeTime, xFrequency );        
    }
}

static void *uartSendTask( void * pvParameters )
{
    TickType_t xLastWakeTime;
    /* Remove compiler warnings in the case where configASSERT() is not defined. */
    ( void ) pvParameters;

    const TickType_t xFrequency = 500;
    
    // Initialise the xLastWakeTime variable with the current time.

    xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
       printf("CollinsAerospace\n\r");
       // Wait for the next cycle.
       vTaskDelayUntil( &xLastWakeTime, xFrequency );        
    }
}

static void *WASMTask( void * pvParameters )
{
    TickType_t xLastWakeTime;
    /* Remove compiler warnings in the case where configASSERT() is not defined. */
    ( void ) pvParameters;

    const TickType_t xFrequency = 1;
    
    // Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();
   
    while(1)
    {
       // Wait for the next cycle.
       vTaskDelayUntil( &xLastWakeTime, xFrequency );        
    }
}

/*
 * Create the demo tasks then start the scheduler.
 */
int main( void )
{
    /* Prepare the hardware to run this demo. */
    prvSetupHardware();
    initWAMR();
    pthread_create( &thread_id_GreenLed, NULL, blinkLEDGreen, NULL );
    pthread_create( &thread_id_AmberLed, NULL, blinkLEDAmber, NULL );    
    pthread_create( &thread_id_uart, NULL, uartSendTask, NULL );
    pthread_create( &thread_id_WASM, NULL, WASMTask, NULL );
    /* Start the created tasks running. */
    vTaskStartScheduler();
    
    /* Execution will only reach here if there was insufficient heap to
    start the scheduler. */
    for( ;; );
    
    return 0;
}