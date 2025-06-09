/*
 * Simple UART console driver for PIC32
 *
 * Copyright (c) 2016 John Robertson
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UART2_H
#define UART2_H

#include <stdbool.h>

typedef struct {
    uint16_t fifoOverruns;
    uint16_t badParity;
    uint16_t framingErrors;
    uint16_t queueFull;
} uart_stats_t;

void Uart2Initialise(uint32_t baud);
void Uart2Enable(void);
void Uart2GetStats(uart_stats_t *pStats);
void Uart2Flush(void);
void Uart2FlushInput(void);
void _mon_putc(char c);

// User interface helpers
int WaitForAKeyPress(TickType_t maxWait);
int InputString(size_t bufSize, char *pBuffer);
bool InputHexValue(size_t bufSize, char *pBuffer, uint32_t *pValue);
bool InputFloatValue(size_t bufSize, char *pBuffer, float *pValue);
bool InputIntValue(size_t bufSize, char *pBuffer, int32_t *pValue);
bool InputIPv4Address(size_t bufSize, char *pBuffer, uint32_t *pAddrN);

#endif // UART2_H
