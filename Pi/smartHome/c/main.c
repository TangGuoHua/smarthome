/*
 * untitled.c
 * 
 * Copyright 2014  <pi@pi2>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 * gcc -Wall -lwiringPi -o "main" "main.c" "nrf24L01.c"
 * 
 */


#include <stdio.h>
#include <unistd.h>
//#include <wiringPi.h>
#include "nrf24L01.h"


void sendDataToHost( )
{
	unsigned char sendData[4];
	unsigned char toAddr[3]= {97, 83, 41}; //Pi's address is {53, 69, 149};
	unsigned char tmp;

		
	sendData[0] = 1;
	sendData[1] = 0;
	sendData[2] = 0;
	sendData[3] = 0; 


	//tmp = nrfSendData( 96, 3, toAddr, 16, sendData);
	tmp = nrfSendData( 92, 3, toAddr, 4, sendData);
	
	printf( "\n\rSend Result=%d\n\r", tmp);
	

}

void startRecv( void )
{
	unsigned char myAddr[3] = {53,70, 132};
	nrfSetRxMode( 99, 3, myAddr );
	
	printf( "Starting listening... \n\r" );
	
}

int main(int argc, char **argv)
{
	unsigned char *receivedData;
	
	printf( "Test NRF24L01+ ... \n\r" );
	
	nrf24L01Init();
	
	sendDataToHost();
	
	startRecv();
	
	while(1)
	{
		if( nrfIsDataReceived() )
		{
			printf( "Data Received. " );
			receivedData = nrfGetReceivedData();
			printf( "D1=%d, D2=%d, D3=%d, D4=%d, D5=%d D6=%d\n\r", *(receivedData++), *(receivedData++),*(receivedData++),*(receivedData++),*(receivedData++),*(receivedData++) );
			
		}
			
		usleep(10000);
	}

	return 0;

}

