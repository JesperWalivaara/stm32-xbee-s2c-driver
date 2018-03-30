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

#ifndef XBEE_S2C_LIB_INC_XBEELIB_H_
#define XBEE_S2C_LIB_INC_XBEELIB_H_

#include "stm32f3xx_hal.h"
#include "string.h"
#include "stdbool.h"

/*
 * GENERAL SETTINGS
 * MODIFY TO FIT YOUR APPLICATION
 */

#define MAX_STORED_DEVICES 5

typedef enum {
	XBEE_OK = 0x0,
	XBEE_UART_FAILED_SYNC = 0x1,
	XBEE_GET_LOCAL_SETTINGS_FAILURE = 0x2
} XBEE_STAT;

/*
 * This struct contains storage locations for
 * all of the different Xbee settings. They have been listed
 * exactly as specified in the user manual.
 *
 * Refer to the document "XBee/Xbee-PRO S2C 802.15.4 RF Module User Guide"
 * found at the link:
 * https://www.digi.com/resources/documentation/Digidocs/90001500/Default.htm
 * For more details
 */
typedef struct {
	// +++ Special Commands +++
	// CMD: WR	(Write to non-volatile memory)
	// CMD: RE	(Factory Reset)
	// CMD: FR	(Software Reset)

	// +++ Networking and security +++
	uint8_t C8;		// 802.15.4 Compatibility
	uint8_t CH;		// Operating Channel
	uint16_t ID;	// Network ID
	uint32_t DH;	// Destination Address High
	uint32_t DL;	// Destination Address Low
	uint16_t MY;	// Source Address
	uint32_t SH;	// IEE 64-bit extended address High
	uint32_t SL;	// IEE 64-bit extended address Low
	uint8_t MM;		// MAC Mode
	uint8_t RR;		// XBee Retries
	uint8_t RN;		// Random Delay Slots
	// CMD: ND (Network Discovery)
	uint8_t NT;		// Node Discover Timeout
	uint8_t NO;		// Node Discovery Options
	// CMD: DN (Discover Node)
	uint8_t CE;		// Coordinator Enable
	uint16_t SC;	// Scan Channels
	uint8_t SD;		// Scan Duration
	uint8_t A1;		// End Device Association
	uint8_t A2;		// Coordinator Association
	// CMD: AI (Association status)
	// CMD: DA (Force Disassociation)
	// CMD: FP (Force Poll)
	// CMD: AS (Beacon request)
	// CMD: ED (Energy Detect)
	uint8_t EE;		// Encryption Enable
	// CMD: KY (AES Encryption Key, write only)
	uint8_t NI[20];	// Node Identifier

	// +++ RF Interfacing Commands +++
	uint8_t PL;		// TX Power Level
	uint8_t PM;		// Power Mode
	uint8_t CA;		// CCA Threshold

	// +++ Sleep Commands +++
	uint8_t SM;		// Sleep Mode
	uint16_t ST;	// Time Before Sleep
	uint16_t SP;	// Cyclic Sleep Period
	uint16_t DP;	// Disassociated Cyclic Sleep Period
	uint8_t SO;		// Sleep Options

	// +++ Serial Interfacing Commands +++
	uint32_t BD;	// Interface Data Rate
	uint8_t NB;		// Parity
	uint8_t RO;		// Inter-character Silence
	uint8_t D7;		// DIO7/CTS
	uint8_t D6;		// DIO6/RTS
	uint8_t AP;		// API Mode Enable

	// +++ I/O Settings Commands +++
	uint8_t D0;		// DIO0/AD0
	uint8_t D1;		// DIO1/AD1
	uint8_t D2;		// DIO2/AD2
	uint8_t D3;		// DIO3/AD3
	uint8_t D4;		// DIO4
	uint8_t D5;		// DIO5/ASSOCIATED_INDICATOR
	uint8_t D8;		// DI8/DTR/SLP_RQ
	uint8_t P0;		// RSSI/PWM0
	uint8_t P1;		// PWM1
	uint8_t P2;		// SPI_MISO
	uint16_t M0;	// PWM0 Duty Cycle
	uint16_t M1;	// PWM1 Duty Cycle
	uint8_t P5;		// SPI_MISO
	uint8_t P6;		// SPI_MOSI
	uint8_t P7;		// SPI_SSEL
	uint8_t P8;		// DIO18/SPI_SCLK
	uint8_t P9;		// SPI_ATTN
	uint8_t PR;		// Pull-up/Down Resistor Enable
	uint8_t PD;		// Pull Up/Down Direction
	uint8_t IU;		// I/O Output Enable
	uint8_t IT;		// Samples before TX
	// CMD: IS	(Force read)
	// CMD: IO	(Set digital output levels)
	uint16_t IC;	// DIO Change Detect
	uint16_t IR;	// Sample Rate
	uint8_t RP;		// RSSI PWM Timer

	// +++ I/O Line Passing Commands +++
	uint32_t IA[2];	// I/O Input Address
	uint8_t T0;		// D0 Timeout
	uint8_t T1;		// D1 Output Timeout
	uint8_t T2;		// D2 Output Timeout
	uint8_t T3;		// D3 Output Timeout
	uint8_t T4;		// D4 Output Timeout
	uint8_t T5;		// D5 Output Timeout
	uint8_t T6;		// D6 Output Timeout
	uint8_t T7;		// D7 Output Timeout
	uint8_t PT;		// PWM Output Timeout

	// +++ Diagnostic Commands +++
	// CMD: VR	(Firmware Version)
	// CMD: VL	(Detailed Version Information)
	// CMD: HV	(Hardware Version Number)
	// CMD: DB	(Last Packet RSSI)
	// CMD: EC	(CCA Failures)
	// CMD: EA	(ACK Failures)
	// CMD:	DD	(Digi Device Type Identifier)

	// +++ Command Mode Options +++
	uint16_t CT;	// Command Mode Timeout (x100 ms)
	// CMD: CN	(Exit Command Mode)
	// CMD: AC	(Apply Changes)
	uint16_t GT;	// Silence Period (x1 ms)
	uint8_t CC;		// Command Character
} xbee_settings;


typedef struct {
	UART_HandleTypeDef *hxbee;
	xbee_settings settings;	// Xbee device settings
} xbee_module;

bool isCoordinator(xbee_module *xbee);
void xbeeSetDefaultValues(xbee_module *xbee);
bool xbeeSyncUART();
bool xbeeGetLocalSettings();
XBEE_STAT xbeeInit();
void xbeeEnterCMDMode();

#endif /* XBEE_S2C_LIB_INC_XBEELIB_H_ */
