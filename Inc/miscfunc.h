/*
 * miscfunc.h
 *
 *  Created on: 22 mars 2018
 *      Author: Jesper
 */

#ifndef MISCFUNC_H_
#define MISCFUNC_H_

#include "xbeelib.h"

typedef struct {
	uint8_t data[200];
	uint16_t datacnt;
	uint16_t size;
} buffer;

//TODO: Remove?
typedef struct {
	uint8_t *data;
	uint16_t datacnt;
	uint16_t size;
} pbuffer;

void platformDelayUs(uint32_t udelay);
bool readAvailableData(UART_HandleTypeDef *huart, buffer *tmp);

void handleTerminalInput(buffer *inp);
void termInit(uint8_t *storage, uint16_t size, UART_HandleTypeDef *huart);

// Public


// Private
void terminalPrintNlCr();
void terminalPrintRightArrow();
void terminalPrintLeftArrow();
void terminalProcessCommandBuffer();


#define MAX_TERM_CMD_LEN 100



#endif /* MISCFUNC_H_ */
