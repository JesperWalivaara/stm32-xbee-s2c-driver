/*
 * miscfunc.c
 *
 *  Created on: 22 mars 2018
 *      Author: Jesper
 */

#include "miscfunc.h"

pbuffer termCmdBuf;
UART_HandleTypeDef *hterm;

/**
 *	Initializes the terminal.
 *
 *	@param *storage, memory location that the terminal can use
 *	@param size, size of storage specified as amount of bytes
 *	@param *huart, STM32 HAL UART handle for the terminal tx/rx
 *
 */
void termInit(uint8_t *storage, uint16_t size, UART_HandleTypeDef *huart)
{
	termCmdBuf.data = storage;
	termCmdBuf.size = size;
	termCmdBuf.datacnt = 0;
	for (int i = 0; i < size; ++i)
	{
		termCmdBuf.data[i] = 0;
	}
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
 *	@param *tmp, pointer to the temporary storage buffer
 *	@return true if data was read, false otherwise
 *
 */
bool readAvailableData(UART_HandleTypeDef *huart, buffer *tmp)
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
		tmp->datacnt = cnt;
		HAL_UART_AbortReceive_IT(huart);
		while(huart->RxState != HAL_UART_STATE_READY)
		{
			// Wait for abort process to complete.
		}

		// Copy over to secondary buffer and re-enable interrupts
		for (int i = 0; i < cnt; ++i) {
			tmp->data[i] = *((huart->pRxBuffPtr-cnt)+i);
		}
		HAL_UART_Receive_IT(huart, (uint8_t*)(huart->pRxBuffPtr-cnt), 200);

		return true;
	}
	return false;
}


/**
 * 	Basic terminal. Will echo typed characters
 * 	back to configured UART when they are added to a command.
 * 	Will process said command when carriage return is sent.
 * 	Lastly it will also erase characters from a command when
 * 	backspace is sent.
 *
 *	@param *inp, received char(s) for terminal to handle
 *
 */
void terminalHandleInput(buffer *inp)
{
	char last = inp->data[inp->datacnt-1];

	// Backspace
	if((last == 0x8) && (termCmdBuf.datacnt > 0))
	{
		char bkspace = 0x08;
		uint8_t msg[5];
		uint8_t len = sprintf((char*)msg,"%c %c", bkspace, bkspace);
		HAL_UART_Transmit(hterm, msg, len, 50);
		--termCmdBuf.datacnt;
	}
	// Leaving room for NULL char
	else if((termCmdBuf.datacnt+inp->datacnt) <= (termCmdBuf.size-1))
	{
		for (int i = 0; i < inp->datacnt; ++i)
		{
			termCmdBuf.data[termCmdBuf.datacnt+i] = inp->data[0+i];
		}
		termCmdBuf.datacnt += inp->datacnt;
		HAL_UART_Transmit(hterm, inp->data, inp->datacnt, 50);
	}


	// If last char is carriage return, proceed to process command.
	if(last == 0xD)
	{
		// Remove \r
		termCmdBuf.data[termCmdBuf.datacnt-1] = 0x0;
		--termCmdBuf.datacnt;

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
	HAL_UART_Transmit(hterm, termCmdBuf.data, termCmdBuf.datacnt, 50);
	terminalPrintNlCr();

	// Ensure NULL char at end
	termCmdBuf.data[termCmdBuf.datacnt] = 0x0;
	if(!strncmp((char *)termCmdBuf.data, "AT", 2 ))
	{
		// Generate Xbee AT-CMD
		terminalPrintLeftArrow();
		uint8_t msg[20];
		uint16_t len = sprintf((char *)msg, "TEMP RESPONSE");
		HAL_UART_Transmit(hterm, msg, len, 50);
	}


	terminalPrintNlCr();
	terminalPrintRightArrow();
	termCmdBuf.datacnt = 0;
}
