/*
 * global.h
 *
 *  Created on: Feb 1, 2013
 *      Author: kevinklug
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <stdint.h>

typedef unsigned char bool;

#define true 1
#define false 0
#define STOP_BITS 2
#define RST_TA_R (TAR = 0x00)

#define TX_DELAY 200

//  Toggle rate = 8e6/(2* CCR0 - 1) = 4e6Hz
//  Generated sine = 4e6 / 16
//at CCR0 = 400 1.255kHz
#define FREQ1 410

//at CCR0 = 227 2.442kHz
#define FREQ2 230

//Sine wave look up table for afsk generation
//static const short lut[16] = {0,  2,  4,  7, 8, 11, 13, 15,
//	                     	15, 14, 12,  9, 6,  3,  1,  0};
//};
//This is half a period.
//static const short lut[16] = {0, 3, 6, 9, 11, 13, 14, 15, 15,
//							14, 13, 11, 9, 6, 3, 0};
static const short lut[16] = {7, 11, 13, 15, 15, 14, 12, 9, 6,
							  3, 1, 0, 0, 2, 4, 7};

//FCS value
extern uint16_t crc;

//Phase Delta for generating sine wave
extern uint8_t Phase_Delta;

//Variables that control AFSK generation
extern uint8_t sent_periods;
extern bool bit_sent;
extern uint8_t cycles;
static uint16_t prev_freq = FREQ1;


typedef struct {
// AX.25 UI-Frame Format
// *********************
// Destination Address
  char *CALL_D;
  uint8_t  CALL_D_SSID;
// Source Address
  char *CALL_S;
  uint8_t CALL_S_SSID;
// Digipeater Address
// Control Field
  uint8_t Control;
// Protocol ID
  uint8_t PID;
// Frame Check Sequence
} aprs_header;



//Variables that control GPS
//volatile extern uint8_t gpgga_detect;
//extern char temp_uart[100];

/*
extern uint8_t i = 0;
extern uint8_t j = 0;
extern uint8_t k = 0;
extern uint8_t wait = 0;
*/

extern volatile bool sleep_on;

#endif /* GLOBAL_H_ */
