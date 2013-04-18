#include <msp430G2553.h>
#include <string.h>
#include <stdint.h>
#include "global.h"
#include "gps.h"
#include "sleep.h"
#include "ax25.h"

bool sleep;

uint8_t main ( void ){
  char *gpgga;
  aprs_header header;
  header.CALL_D = "APRS  ";
  header.CALL_D_SSID = 0xE0;
  header.CALL_S = "KF7IYK";
  header.CALL_S_SSID = 0x47;
  header.Control = 0xC0;
  header.PID = 0x0F;

  //P2REN |= BIT1;
  P2DIR |= BIT1;
  P2OUT &= ~BIT1;
  while(1){
    //GPGGA will contain the entire GPGGA string
    //that is received from the gps.
    //GPS_wakeup();
    do{
      GPS_setup();
      gpgga = GPS_aquire();
      ax_timer_setup();

    }while(!send_packet( header, gpgga ));
    P2OUT = 0x00;
    Sleep();

  }
}
