/*
Copyright 2018 Jesper Wälivaara

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

#ifndef MISCFUNC_H_
#define MISCFUNC_H_

#include "xbeelib.h"

typedef struct {
	uint8_t *data;
	uint16_t datacnt;
	uint16_t size;
} buffer;

void platformDelayUs(uint32_t udelay);
bool readAvailableData(UART_HandleTypeDef *huart, buffer *secbuf);

void handleTerminalInput(buffer *inp);
void termInit(buffer *termdatabuf, UART_HandleTypeDef *huart);

// Public


// Private
void terminalPrintNlCr();
void terminalPrintRightArrow();
void terminalPrintLeftArrow();
void terminalProcessCommandBuffer();


#define MAX_TERM_CMD_LEN 100
#define UART_RXBUF_SIZE 200



#endif /* MISCFUNC_H_ */
