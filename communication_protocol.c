/*
 * Protocoles de communication, trames
 *
 *   XBee, FM, GSM
 *
 */
 
#include "communication_protocol.h"

// Xbee global variables
int XBeePacketReceived = 0;
int XBeeIdByteReceived = 0;
unsigned char XBeePacket[XBEE_PACKET_SIZE];
XBeeFrame XBeeSkeleton ;

// FM global variables
int FMPacketReceived = 0;
int FMIdByteReceived = 0;
unsigned char FMPacket[FM_PACKET_SIZE];





/* Add a byte to the packet being filled.
 * Returns 0 if the packet is not fully filled, 1 either.
 */
int addReceivedByte(char byte, ID_UART uart)
{
	
	int packetEnded = 0;
	
	switch(uart)
	{
		
		case XBEE:
		{
			XBeePacket[XBeeIdByteReceived] = byte;
			XBeeIdByteReceived++;
			if (XBeeIdByteReceived == XBEE_PACKET_SIZE)
			{
				XBeeIdByteReceived = 0;
				packetEnded = 1;
			}
		} break;
		
		case FM:
		{
			
		} break;
		
		case GSM:
		{
			
		} break;
		
	}
	
	return packetEnded;

}

/** Translate the bytes of the filled packet.
 * Returns a structure containing all the fields.
 */
void decodePacket(ID_UART uart)
{
	
	//SendString(XBeePacket, XBEE_PACKET_SIZE, XBEE);
	
	switch(uart)
	{
		
		case XBEE:
		{
			
			XBeeSkeleton.type = XBeePacket[0] ;
			XBeeSkeleton.num_seq = XBeePacket[1] ;
			XBeeSkeleton.source = XBeePacket[3] ;
			XBeeSkeleton.source = (XBeeSkeleton.source | ( ((short int) XBeePacket[2]) << 8 ) ) ;
			XBeeSkeleton.dest = XBeePacket[5] ;
			XBeeSkeleton.dest = (XBeeSkeleton.dest | ( ((short int) XBeePacket[4]) << 8 ) ) ;
			XBeeSkeleton.count = XBeePacket[6] ;
			XBeeSkeleton.data = XBeePacket[10] ;
			XBeeSkeleton.data = (XBeeSkeleton.data | ( ((long int) XBeePacket[9]) << 8 ) ) ;
			XBeeSkeleton.data = (XBeeSkeleton.data | ( ((long int) XBeePacket[8]) << 16 ) ) ;
			XBeeSkeleton.data = (XBeeSkeleton.data | ( ((long int) XBeePacket[7]) << 24 ) ) ;
			
			
		} break;
		
		case FM:
		{
			
		} break;
		
		case GSM:
		{
			
		} break;
		
	}
	
}

/** Send a frame from a source to a destination
 * Returns an integer : 0 if transmission done, 1 if an error occurs
 */
void sendPacket(ID_UART uart, unsigned char type, unsigned char num_seq, short int source, short int dest, unsigned char count, long int data)
{
		switch(uart)
	{
		
		case XBEE:
		{
			unsigned char XBeePacketToSend[XBEE_PACKET_SIZE] ;
			
			XBeePacketToSend[0] = SYNC_CHAR_1 ;
			XBeePacketToSend[1] = SYNC_CHAR_2 ;
			XBeePacketToSend[2] = type ;
			XBeePacketToSend[3] = num_seq ;
			XBeePacketToSend[4] = source ;
			XBeePacketToSend[5] = ( source >> 8 ) ;
			XBeePacketToSend[6] = dest  ;
			XBeePacketToSend[7] = ( dest >> 8 ) ;
			XBeePacketToSend[8] = count ;
			XBeePacketToSend[9] = data ;
			XBeePacketToSend[10] = ( data >> 8 ) ;
			XBeePacketToSend[11] = ( data >> 16 ) ;
			XBeePacketToSend[12] = ( data >> 24 ) ;
			
			SendString(XBeePacketToSend, XBEE_PACKET_SIZE, XBEE);

		} break;
		
		case FM:
		{
			
		} break;
		
		case GSM:
		{
			
		} break;
		
	}
}
