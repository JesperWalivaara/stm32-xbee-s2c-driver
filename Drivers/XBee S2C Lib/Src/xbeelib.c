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

#include "xbeelib.h"

// xbee[0] is always going to be the local device
// any additional devices will be remote nodes
xbee_module xbee[MAX_STORED_DEVICES];


// use XBEE_IS_USING_SPI, XBEE_IS_USING_UART

/*
 * Determines if target xbee is a network coordinator
 *
 * @param *xbee, handle for target xbee
 * @retval true or false
 */
bool isCoordinator(xbee_module *xbee)
{
	// CE = 0 => xbee is end device
	// CE = 1 => xbee is the coordinator
	return (xbee->settings.CE);
}


XBEE_STAT xbeeInit(UART_HandleTypeDef *hxbee)
{
	for(int i = 0; i < MAX_STORED_DEVICES; ++i)
	{
		xbeeSetDefaultValues(&xbee[i]);
	}
	xbee[0].hxbee = hxbee;

	// Synchronize UART with the local Xbee module
	if(xbeeSyncUART())
	{
		// Fetch local Xbee module's settings
		if(xbeeGetLocalSettings())
		{

		}
		else
		{
			return XBEE_GET_LOCAL_SETTINGS_FAILURE;
		}
	}
	else
	{
		return XBEE_UART_FAILED_SYNC;
	}

	return XBEE_OK;
}

/*
 *	Initializes the xbee_module struct with some default values.
 *
 *	@param *xbee, handle for target xbee module
 */
void xbeeSetDefaultValues(xbee_module *xbee)
{
	// +++ Networking and security +++
	xbee->settings.C8 = 0;		// 802.15.4 Compatibility
	xbee->settings.CH = 0;		// Operating Channel
	xbee->settings.ID = 0;		// Network ID
	xbee->settings.DH = 0;		// Destination Address High
	xbee->settings.DL = 0;		// Destination Address Low
	xbee->settings.MY = 0;		// Source Address
	xbee->settings.SH = 0;		// IEE 64-bit extended address High
	xbee->settings.SL = 0;		// IEE 64-bit extended address Low
	xbee->settings.MM = 0;		// MAC Mode
	xbee->settings.RR = 0;		// XBee Retries
	xbee->settings.RN = 0;		// Random Delay Slots
	xbee->settings.NT = 0;		// Node Discover Timeout
	xbee->settings.NO = 0;		// Node Discovery Options
	xbee->settings.CE = 0;		// Coordinator Enable
	xbee->settings.SC = 0;		// Scan Channels
	xbee->settings.SD = 0;		// Scan Duration
	xbee->settings.A1 = 0;		// End Device Association
	xbee->settings.A2 = 0;		// Coordinator Association
	xbee->settings.EE = 0;		// Encryption Enable
	for(int i = 0; i < 20; ++i)
	{
		xbee->settings.NI[i] = 0;
	}

	// +++ RF Interfacing Commands +++
	xbee->settings.PL = 0;		// TX Power Level
	xbee->settings.PM = 0;		// Power Mode
	xbee->settings.CA = 0;		// CCA Threshold

	// +++ Sleep Commands +++
	xbee->settings.SM = 0;		// Sleep Mode
	xbee->settings.ST = 0;		// Time Before Sleep
	xbee->settings.SP = 0;		// Cyclic Sleep Period
	xbee->settings.DP = 0;		// Disassociated Cyclic Sleep Period
	xbee->settings.SO = 0;		// Sleep Options

	// +++ Serial Interfacing Commands +++
	xbee->settings.BD = 0;		// Interface Data Rate
	xbee->settings.NB = 0;		// Parity
	xbee->settings.RO = 0;		// Inter-character Silence
	xbee->settings.D7 = 0;		// DIO7/CTS
	xbee->settings.D6 = 0;		// DIO6/RTS
	xbee->settings.AP = 0;		// API Mode Enable

	// +++ I/O Settings Commands +++
	xbee->settings.D0 = 0;		// DIO0/AD0
	xbee->settings.D1 = 0;		// DIO1/AD1
	xbee->settings.D2 = 0;		// DIO2/AD2
	xbee->settings.D3 = 0;		// DIO3/AD3
	xbee->settings.D4 = 0;		// DIO4
	xbee->settings.D5 = 0;		// DIO5/ASSOCIATED_INDICATOR
	xbee->settings.D8 = 0;		// DI8/DTR/SLP_RQ
	xbee->settings.P0 = 0;		// RSSI/PWM0
	xbee->settings.P1 = 0;		// PWM1
	xbee->settings.P2 = 0;		// SPI_MISO
	xbee->settings.M0 = 0;		// PWM0 Duty Cycle
	xbee->settings.M1 = 0;		// PWM1 Duty Cycle
	xbee->settings.P5 = 0;		// SPI_MISO
	xbee->settings.P6 = 0;		// SPI_MOSI
	xbee->settings.P7 = 0;		// SPI_SSEL
	xbee->settings.P8 = 0;		// DIO18/SPI_SCLK
	xbee->settings.P9 = 0;		// SPI_ATTN
	xbee->settings.PR = 0;		// Pull-up/Down Resistor Enable
	xbee->settings.PD = 0;		// Pull Up/Down Direction
	xbee->settings.IU = 0;		// I/O Output Enable
	xbee->settings.IT = 0;		// Samples before TX
	xbee->settings.IC = 0;		// DIO Change Detect
	xbee->settings.IR = 0;		// Sample Rate
	xbee->settings.RP = 0;		// RSSI PWM Timer

	// +++ I/O Line Passing Commands +++
	xbee->settings.IA[0] = 0;
 	xbee->settings.IA[1] = 0;	// I/O Input Address
	xbee->settings.T0 = 0;		// D0 Timeout
	xbee->settings.T1 = 0;		// D1 Output Timeout
	xbee->settings.T2 = 0;		// D2 Output Timeout
	xbee->settings.T3 = 0;		// D3 Output Timeout
	xbee->settings.T4 = 0;		// D4 Output Timeout
	xbee->settings.T5 = 0;		// D5 Output Timeout
	xbee->settings.T6 = 0;		// D6 Output Timeout
	xbee->settings.T7 = 0;		// D7 Output Timeout
	xbee->settings.PT = 0;		// PWM Output Timeout

	// +++ Command Mode Options +++
	xbee->settings.CT = 0x64;   // Command Mode Timeout
	xbee->settings.GT = 0x3E8;	// Silence Period
	xbee->settings.CC = 0x2B;	// Command Character
}


bool xbeeSyncUART()
{
	// Check at current baud
	// 1. Send ATBD, is rec = uart?
	// 2. no? continue function
	// 3. yes? save val, then exit function

	//if fail some error code should be present

	// loop through list of "standard" rates
	// LOOP:
	// 1. Send ATBD, is rec = uart?
	// 2. no? go to next one
	// 3. yes? save val, then exit function
	return true;
}


bool xbeeGetLocalSettings()
{
	return true;
}

/*
 *	Sets the local Xbee module into command mode.
 *	In most use cases should only really be used during the initialization stage.
 *	Reconfiguring of any AT parameters is best done via API commands.
 *	This function is therefore only used when the settings of the
 *	local Xbee are unknown. The Xbee module can ALWAYS enter command mode,
 *	even when API mode is enabled (assuming matching UART baud rates).
 *
 */
void xbeeEnterCMDMode()
{
	uint8_t tmp = xbee[0].settings.CC;
	uint8_t cmdsequence[3] = {tmp,tmp,tmp};

	// GT + 3xCC + GT
	HAL_Delay(xbee[0].settings.GT);
	HAL_UART_Transmit(xbee[0].hxbee, cmdsequence, 3, 10);
	HAL_Delay(xbee[0].settings.GT);
}
