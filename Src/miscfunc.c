/*
Copyright 2018 Jesper W�livaara

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is furnished to
do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies
or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "miscfunc.h"

buffer *termCache;
UART_HandleTypeDef *hterm;

/**
 *	Initializes the terminal.
 *
 *	@param *termCache, data buffer that the terminal can use
 *	@param *huart, STM32 HAL UART handle for the connection going to the terminal
 */
void termInit(buffer *termbuf, UART_HandleTypeDef *huart)
{
	termCache = termbuf;
	hterm = huart;

	char msg[50];
	uint16_t len;
	len = sprintf(msg, "\r\nTERMINAL INITIALIZED\r\n");
	HAL_UART_Transmit(hterm, (uint8_t*)msg, len, 50);
	len = sprintf(msg, "=====================\r\n");
	HAL_UART_Transmit(hterm, (uint8_t*)msg, len, 50);
	terminalPrintRightArrow();
}


/**
 * Creates a blocking mode delay of 'udelay' microseconds.
 * At systemclock 64 Mhz, Prescaler TIM2->PSC = 64
 * => Tim2 updates every 1 us. NOTE: Requires TIM2 to be enabled in
 * HAL Drivers with its clock source set to internal.
 */
void platformDelayUs(uint32_t udelay)
{
	/* 1. Clear update event flag
	 *2. Set autoreload value
	 *3. Enable timer
	 *4. Wait until timer counts to udelay
	 *5. Disable timer
	 */
	TIM2->SR = 0;
	TIM2->ARR = udelay;
	TIM2->CR1 |= TIM_CR1_CEN;
	while(!(TIM2->SR & TIM_SR_UIF));
	TIM2->CR1 &= ~TIM_CR1_CEN;
}


/**
 *	Reads available data from target UART and copies it over
 *	to a temporary buffer. This function works with UART interrupts
 *	and utilizes a timeout process to terminate data reception.
 *
 *	@param *huart, STM HAL library handle for target uart interface
 *	@param *secbuf, pointer to the secondary storage buffer
 *	@return true if data was read and copied, false otherwise
 *
 */
bool readAvailableData(UART_HandleTypeDef *huart, buffer *secbuf)
{
	//TODO: Tweak timeout values
	uint32_t timeout_usmax = 10;
	uint32_t timeout_ticks = 0;
	uint32_t timeout_usdelay = 100;
	uint16_t savedval = 0;

	// These values will differ only if data
	// has been received on the Rx pin.
	if (huart->RxXferCount != huart->RxXferSize)
	{
		while (timeout_ticks < timeout_usmax)
		{
			if(savedval != huart->RxXferCount)
			{
				savedval = huart->RxXferCount;
			}
			else
			{
				platformDelayUs(timeout_usdelay);
				++timeout_ticks;
			}
		}

		uint16_t cnt = huart->RxXferSize-huart->RxXferCount;
		HAL_UART_AbortReceive_IT(huart);
		while(huart->RxState != HAL_UART_STATE_READY)
		{
			// Wait for abort process to complete.
		}

		if (cnt > secbuf->size)
		{
			// Received data will not fit in secondary buffer
			// therefore do not copy over
			HAL_UART_Receive_IT(huart, (uint8_t*)(huart->pRxBuffPtr-cnt), huart->RxXferSize);
			secbuf->datacnt = 0;
			return false;
		}
		else
		{
			// Copy over to secondary buffer and re-enable interrupts
			for (int i = 0; i < cnt; ++i) {
				secbuf->data[i] = *((huart->pRxBuffPtr-cnt)+i);
			}
			HAL_UART_Receive_IT(huart, (uint8_t*)(huart->pRxBuffPtr-cnt), huart->RxXferSize);
			secbuf->datacnt = cnt;
			return true;
		}
	}
	return false;
}


/**
 * 	Basic terminal behavior on character input.
 * 	Will echo typed characters back to configured UART when
 * 	they are added to a command. Will process said command when
 * 	carriage return is sent. Lastly it will also erase characters
 * 	from a command when	backspace is sent.
 *
 *	@param *inp, received char(s) for terminal to handle
 *
 */
void handleTerminalInput(buffer *inp)
{
	char last = inp->data[inp->datacnt-1];

	// Backspace
	if((last == 0x8) && (termCache->datacnt > 0))
	{
		char bkspace = 0x08;
		uint8_t msg[5];
		uint8_t len = sprintf((char*)msg,"%c %c", bkspace, bkspace);
		HAL_UART_Transmit(hterm, msg, len, 50);
		--termCache->datacnt;
	}
	// Leaving room for NULL char
	else if((termCache->datacnt+inp->datacnt) <= (termCache->size-1))
	{
		for (int i = 0; i < inp->datacnt; ++i)
		{
			termCache->data[termCache->datacnt+i] = inp->data[0+i];
		}
		termCache->datacnt += inp->datacnt;
		HAL_UART_Transmit(hterm, inp->data, inp->datacnt, 50);
	}


	// If last char is carriage return, proceed to process command.
	if(last == 0xD)
	{
		// Remove \r
		termCache->data[termCache->datacnt-1] = 0x0;
		--termCache->datacnt;

		terminalProcessCommandBuffer();
	}
}


/**
 * 	Prints command sequence Newline+Carriage return
 * 	to the terminals uart.
 */
void terminalPrintNlCr()
{
	uint8_t nl = '\n';
	uint8_t cr = '\r';
	uint8_t nlcr[2];
	sprintf((char*)nlcr, "%c%c", nl, cr);
	HAL_UART_Transmit(hterm, nlcr, 2, 10);
}


/**
 * 	Prints command sequence "> "
 * 	to the terminals uart.
 */
void terminalPrintRightArrow()
{
	uint8_t arrow[2];
	sprintf((char*)arrow, "> ");
	HAL_UART_Transmit(hterm, arrow, 2, 10);
}


/**
 * 	Prints command sequence "> "
 * 	to the terminals uart.
 */
void terminalPrintLeftArrow()
{
	uint8_t arrow[2];
	sprintf((char*)arrow, "< ");
	HAL_UART_Transmit(hterm, arrow, 2, 10);
}


void terminalProcessCommandBuffer()
{
	// Echo command buffer contents
	terminalPrintNlCr();
	HAL_UART_Transmit(hterm, termCache->data, termCache->datacnt, 50);
	terminalPrintNlCr();

	// Ensure NULL char at end
	termCache->data[termCache->datacnt] = 0x0;
	if(!strncmp((char *)termCache->data, "AT", 2 ))
	{
		// Generate Xbee AT-CMD
		terminalPrintLeftArrow();
		uint8_t msg[20];
		uint16_t len = sprintf((char *)msg, "TEMP RESPONSE");
		HAL_UART_Transmit(hterm, msg, len, 50);
	}

	terminalPrintNlCr();
	terminalPrintRightArrow();
	termCache->datacnt = 0;
}



