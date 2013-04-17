/*
 * gps.h
 *
 *  Created on: April 4, 2013
 *      Author: hao c & kevin klug
 */

#ifndef GPS_H_
#define GPS_H_

//Changes MSP430 clock to internal at 8MHz
//TODO: Implement ON/OFF switch
void GPS_setup( void );

//Checks GPS string for valid GPS Position fix
bool GPS_fix( char* gpgga_string);

//Waits for a valid GPS signal
char* GPS_aquire( void );

//Toggles the gps ON/OFF pin
void GPS_toggle( void );

#endif /* GPS_H_ */
