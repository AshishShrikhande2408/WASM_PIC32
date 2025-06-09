/*
 * FreeRTOS V202212.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/*
 * This file implements functions to access and manipulate the PIC32 hardware
 * without reliance on third party library functions that may be liable to
 * change.
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"

/* Demo includes. */
#include "init.h"
#include "wasm_runtime_common.h"

#define hwUNLOCK_KEY_0					( 0xAA996655UL )
#define hwUNLOCK_KEY_1					( 0x556699AAUL )
#define ptOUTPUT 	0
#define ptALL_OFF	0
#define ptNUM_LEDS	5

/* all the runtime memory allocations are retricted in the global_heap_buf array */
static char global_heap_buf[256 * 1024];
RuntimeInitArgs init_args;

/*-----------------------------------------------------------*/

void vHardwareConfigurePerformance( void )
{
	/* set PBCLK2 to deliver 40Mhz clock for PMP/I2C/UART/SPI. */
	SYSKEY = hwUNLOCK_KEY_0;
	SYSKEY = hwUNLOCK_KEY_1;

	/* 200MHz / 5 = 40MHz */
	PB2DIVbits.PBDIV = 0b100;

	/* Timers use clock PBCLK3, set this to 40MHz. */
	PB3DIVbits.PBDIV = 0b100;

	/* Ports use PBCLK4. */
	PB4DIVbits.PBDIV = 0b000;

	SYSKEY = 0;

	/* Disable interrupts - note taskDISABLE_INTERRUPTS() cannot be used here as
	FreeRTOS does not globally disable interrupt. */
	__builtin_disable_interrupts();
}
/*-----------------------------------------------------------*/

void vHardwareUseMultiVectoredInterrupts( void )
{
	/* Enable multi-vector interrupts. */
	_CP0_BIS_CAUSE( 0x00800000U );
	INTCONSET = _INTCON_MVEC_MASK;
	__builtin_enable_interrupts();
}

/*-----------------------------------------------------------
 * Simple parallel port IO routines.
 *-----------------------------------------------------------*/

void vParTestInitialise( void )
{
	/* All LEDs output. */
	TRISE = ptOUTPUT;
	LATE = ptALL_OFF;
}

void initWAMR(void)
{
    memset(&init_args, 0, sizeof(RuntimeInitArgs));

    /* configure the memory allocator for the runtime */
    init_args.mem_alloc_type = Alloc_With_Pool;
    init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
    init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);
    init_args.running_mode = Mode_Interp;
    /* set maximum thread number if needed when multi-thread is enabled,
       the default value is 4 */
    init_args.max_thread_num = 4;      

    /* initialize runtime environment with user configurations*/
    if (!wasm_runtime_full_init(&init_args)) 
    {
        printf("WAMR Init failed!!\n\r");
    }
    else
    {
        printf("WAMR Init successful!!\n\r");
    }   
    wasm_runtime_set_log_level(WASM_LOG_LEVEL_VERBOSE);
}





