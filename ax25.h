/*
 * ax25.h
 * 
 *  Implements the transmission of AFSK data through a MSP430
 *
 *  Created on: Feb 1, 2013
 *      Author: kevinklug
 */

#ifndef AX25_H_
#define AX25_H_

#include "global.h"
#include <stdint.h>
#include <msp430.h>

#define send_flag send_byte('~')

//Initalizes MSP430 registers for transmission of APRS packets
void ax_timer_setup( void );

//Enables TimerA interrupts
void begin_tx ( void );

//Disables TimerA interrupts
void stop_tx ( void );

//update_crc will update the global 16 bit checksum with the byte to be sent
// according to the CRC-CCITT standard. [With inital crc = 0xFFFF]
inline void update_crc ( bool bit ){
	//This algorithm was taken from
	//trackuino EA5HAV Javi project
	crc ^= bit;
	if (crc & 1)
		crc = (crc >>1) ^ 0x8408;
	else
		crc = crc >>1;
}

//flip_crc changes the order of the FCS to be LSB first
void finalize_crc ( void );

//This function will set the timerA registers to the appropriate frequency. 
//  It will then wait until one bit is sent.
//NOTE: This function will already NRZI encode the bits. That is, a series of
//  ones will be a continuous frequency and a zero will be encoded as a change
//  in frequency.
inline void send_bit ( bool bit ){
  begin_tx();

  //  Bell 202 Uses NRZI encoding which means a 0 is a change in tone
  //  and a 1 is a continuous tone.
  if (!bit){
    if (prev_freq == FREQ1){
      prev_freq = FREQ2;
    }else{
      prev_freq = FREQ1;
    }
  }
  TA1CCR0 = prev_freq;
  //Toggles led to logic state
  //P1OUT = !(P1OUT & BIT6) ? BIT6 : 0x00;
  //Wait for the bit to be sent
  while(!bit_sent){};
  bit_sent = false;
  stop_tx();
}

inline void send_byte ( char byte ){
  uint8_t byte_index = 0;
  uint8_t stuffer = 0;
  bool bit;
  bool Flag_flag = false;

  //Check if the byte being sent is the Flag 01111110
  //otherwise update the CRC
  if (byte == 0x7E){
    Flag_flag = true;
  }

  //Send bits in least significant bit first
  for ( byte_index = 0; byte_index < 8; byte_index++)
  {
    bit = (byte & (1<<byte_index)) != 0;
    stuffer += bit ? 1 : 0;
    send_bit ( bit );
    if (!Flag_flag) update_crc( bit );
    if (stuffer == 5 && !Flag_flag){
      send_bit( false );
      stuffer = 0;
    }
  }
}

inline void send_string (char* string ){
  uint8_t string_index = 0;
  for ( string_index = 0; string_index < strlen(string); string_index++){
    send_byte (string[string_index]);
  }
}

bool send_packet (aprs_header my_header, char* gpgga_string);

#endif /* AX25_H_ */
