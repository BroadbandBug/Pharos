/*
 * ax25.c
 *
 *  Created on: Feb 1, 2013
 *      Author: kevinklug
 */
#include <msp430.h>
#include <string.h>
#include "ax25.h"

uint8_t Phase_Delta = 0;
uint8_t sent_periods = 0;
bool bit_sent = false;
uint8_t cycles = 0;
uint16_t crc = 0xFFFF;

void ax_timer_setup (void){
  WDTCTL = WDTPW + WDTHOLD;        // Stop watchdog timer

  BCSCTL1 = CALBC1_8MHZ;           // Set range
  BCSCTL2 = 0x00;                  // Change SMCLK divider to 1
  DCOCTL = CALDCO_8MHZ;            // Set DCO step and modulation

  P2DIR = 0xFF;                    // Set Resistor Ladder output pins

  //P1DIR |= BIT0;                   // Set P1.0 to output direction
  //P1OUT &= ~BIT0;                  // Set the red LED off

  //P1DIR |= BIT6;                   // Set P1.6 to output direction
  //P1OUT &= ~BIT6;                  // Set the green LED off

  TA0CCR0 = FREQ1;                 // Count limit (16 bit)
  TA0CTL = TASSEL_2 + MC_1;        // Timer A0 with SMCLK, count UP

  TA1CCR0 = FREQ2;                 // Count limit (16 bit)
  TA1CTL = TASSEL_2 + MC_1;        // Timer A1 with SMCLK, count UP

  IE2 &= ~UCA0RXIE;

  _BIS_SR( GIE );                  // General interrupts enabled

  crc = 0xFFFF;
}

void begin_tx ( void ){
  TA0CCR0 = FREQ1 +10;                 // Count limit (16 bit)
  TA1CCR0 = FREQ1;                 // Count limit (16 bit)
  TA0CCTL0 = CCIE;                 // Enable Timer A0 interrupts, bit 4=1
  TA1CCTL0 = CCIE;                 // Enable Timer A1 interrupts, bit 4=1
  prev_freq = FREQ1;
}

void stop_tx ( void ){
  TA0CCTL0 &= ~CCIE;               // Disable Timer A0 interrupts, bit 4=1
  TA1CCTL0 &= ~CCIE;               // Disable Timer A1 interrupts, bit 4=1
}


void finalize_crc( void ){
	//This was taken from Trackduino 2/21/2013 send_footer function
	// Save the crc so that it can be treated it atomically
	uint16_t final_crc = crc;
  // Send the CRC
	send_byte(~(final_crc & 0xff));
	final_crc >>= 8;
	send_byte(~(final_crc & 0xff));
}

bool send_packet (aprs_header my_header, char* gpgga_string){

  //Check if gpgga_string is valid
  if (*gpgga_string != '$') return false;

  //Transmit several flags to prepare the receiver
  uint8_t num_flag = 0;
  for( num_flag = 0; num_flag < TX_DELAY; num_flag++ ){
    send_flag;
  }

  //Transmit the aprs_header
  send_string( my_header.CALL_D );
  send_byte( my_header.CALL_D_SSID );
  send_string( my_header.CALL_S );
  send_byte( my_header.CALL_S_SSID );
  send_byte( my_header.Control );
  send_byte( my_header.PID );

  //Potential problem with this code. If gpgga_string
  // is not \0 terminated! Check
  send_string( gpgga_string );

  //Send FCS
  finalize_crc();

  //End APRS-UI Frame
  send_flag;
  send_flag;
  send_flag;

  return true;
}
