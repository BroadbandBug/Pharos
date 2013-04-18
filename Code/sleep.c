#include <msp430.h>
#include "sleep.h"
#include "global.h"

volatile uint8_t sleep_counter = 0;
volatile bool sleep_on;

void Sleep( void ){
  WDTCTL = WDTPW + WDTHOLD; // Stop WDT

  BCSCTL1 = CALBC1_8MHZ;
  DCOCTL = CALDCO_8MHZ;

  TA0CCR0 = 511; //?
  TA0CTL = TASSEL_2 +MC_1;
  TA0CCTL0 = CCIE;
  sleep_on = true;
  __enable_interrupt();
  //Stay here while sleeping
  while(sleep_on);
  
  //DEBUGGGING!!
  //P1OUT =0x01; //LED on

  BCSCTL1 = CALBC1_8MHZ;
  DCOCTL = CALDCO_8MHZ;
  TA0CCR0 = 2; 
  TA0CTL = TASSEL_2 +MC_1;  //Switch to DCO
  TA0CCTL0 &= ~CCIE;  //Disable TimerA Interrupt
}

//Timer A0 interrupt service routine
//Sleep Interrupt & AX25 Baudrate timer
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
  //Using TimerA for deep sleep timer
  if (sleep_on){
    if (sleep_counter==0){ // 11428 10min
      sleep_counter++;
      BCSCTL1 |= DIVA_3; // ACLK/8
      BCSCTL3 |= XCAP_3; //12.5pF cap- setting for 32768Hz crystal

      TA0CCR0 = 511; 
      TACTL = TASSEL_1 + ID_3+ MC_1;       // ACLK, /8, upmode
      TA0CCTL0 = CCIE;

      //DEBUGGING
      //P1OUT = 0x00; // P1.0 turn off
      _BIS_SR(LPM3_bits+ GIE); // Enter LPM3 w/interrupt
    }else if (sleep_counter==10) //set time stay in LPM
    {
      sleep_counter=0;
      sleep_on = false;
      _BIC_SR(LPM3_EXIT); //exit LPM
  	}else{
  	  sleep_counter++;
  	}
  }else{
    //This should contain the AX25 interrupt code.
    if(cycles > 15){
      bit_sent = true;
      cycles = 0;
    }
    cycles++;
  }
}
#pragma vector=TIMER1_A0_VECTOR    // Timer0 A0 interrupt service routine
__interrupt void Timer0_A0 (void) {
  P2OUT = lut[Phase_Delta]<<2 | BIT1;
  Phase_Delta++;
  if (Phase_Delta > 15){
    Phase_Delta = 0;
  }
} 
