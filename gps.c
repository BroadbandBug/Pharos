#include "global.h"
#include <msp430.h>
#include <string.h>
#include <stdlib.h>
#include "gps.h"

volatile bool gpgga_detect = false;
volatile bool gps_on = false;
char temp_uart[200];
uint8_t string_index;
uint8_t k;
uint8_t wait;

void delay(unsigned int ms)
{
while (ms>0){
  ms--;
  __delay_cycles(4000);
  }
}

void GPS_setup( void ){
  //Kick the Dog
  WDTCTL = WDTPW + WDTHOLD; // Stop WDT
  if (CALBC1_8MHZ==0xFF){   // If calibration constant erased
    while(1);            // do not load, trap CPU!!
  }

  //Configuration of Clock cycles
  DCOCTL = 0;               // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_8MHZ;    // Set DCO
  DCOCTL = CALDCO_8MHZ;
  UCA0CTL1 |= UCSSEL_2;     // SMCLK
  P1SEL = BIT1 + BIT2;      // P1.1 = RXD, P1.2=TXD
  P1SEL2 = BIT1 + BIT2;     // P1.1 = RXD, P1.2=TXD
  P1DIR = BIT0 + BIT6;      // P1.0 & P1.6 Output
  //P1OUT = BIT0;             // LED1 on

  //Configuration of UART
  UCA0BR0 = 0x82;  // 8MHz 4800 UCA0BR0 = Frequency/baud rate
  UCA0BR1 = 0x06;   // 8MHz 4800
  UCA0MCTL = 0x0c;     // Modulation UCBRSx = 6
  UCA0CTL1 &= ~UCSWRST;   // **Initialize USCI state machine**
  IE2 |= UCA0RXIE;  // Enable USCI_A0 RX interrupt
  __bis_SR_register(GIE); // Enter LPM0, interrupts enabled
}

char* GPS_aquire( void ){
  char* gpgga;
  volatile bool sentence_validity = false;
  volatile uint8_t gpgga_caps = 0;
  //Wake up GPS by toggling on off pin
  gps_on = false;
  do{
    GPS_toggle();
  }while(!gps_on);
  gps_on = false;
  uint64_t test;
  //Delay with interrupts
  for(test = 0; test<4294967; test++);
  while(!gps_on){
	  GPS_toggle();
    //Ensure that the GPS is on
	  for(test = 0; test<4294967; test++);
  }

  do{
    IE2 |= UCA0RXIE;
    if (gpgga_detect){
      IE2 &= ~UCA0RXIE;
      gpgga_caps++;
      gpgga_detect = false;
      //strcpy(gpgga, temp_uart);
      gpgga = (char*)calloc(strlen(temp_uart),sizeof(char));
      strcpy(gpgga, temp_uart);
      sentence_validity = GPS_fix(gpgga);
      if(!sentence_validity) free(gpgga);
    }
    //if (!gps_on) GPS_toggle();
  }while(!sentence_validity);
  //Valid String has been parsed

  //Turn GPS OFF
  GPS_toggle();
  gps_on = false;
  return gpgga;
}

bool GPS_fix( char * gpgga_string){
  char * pcomma;
  bool valid;
  uint8_t num_commas = 0;
  pcomma = strchr(gpgga_string,',');
  while(pcomma != NULL){
    pcomma = strchr(pcomma+1,',');
    num_commas++;
    if( num_commas == 5 ){
      valid = (*(pcomma+1) == '1' || *(pcomma+1) == '2');
      *(pcomma+1)= '0';
      return !valid;
    }
  }
  return false;
}

// USCI A0/B0 Receive ISR
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void){
  gps_on = true;

  if( memcmp(temp_uart, "$GPGGA",6)==0 && UCA0RXBUF == '$' ){
    gpgga_detect = true;
    string_index = 0;
    IE2 &= ~UCA0RXIE;
  }else if(UCA0RXBUF == '$'){
	string_index = 0;
  }
  temp_uart[string_index++] = UCA0RXBUF;
}

void GPS_toggle( void ){
  P1OUT |= BIT0;
  delay(200);
  P1OUT &= ~BIT0;
  delay(1500);
}

