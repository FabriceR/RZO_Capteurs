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
FMFrame FMSkeleton ;

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
			if (++XBeeIdByteReceived == XBEE_PACKET_SIZE-2) // -2 because the first two sync characters are not taken in account
			{
				XBeeIdByteReceived = 0;
				packetEnded = 1;
			}
		} break;
		
		case FM:
		{
			FMPacket[FMIdByteReceived] = byte;
			if (++FMIdByteReceived == FM_PACKET_SIZE-2) // -2 because the first two sync characters are not taken in account
			{
				FMIdByteReceived = 0;
				packetEnded = 1;
			}
		} break;
		
		case GSM:
		{
			
		} break;
		
	}
	
	return packetEnded;

}

/** Translate the bytes of the filled packet.
 * Returns the validity and the origin of the packet.
 */
int decodePacket(ID_UART uart)
{
	int validity = NO_PACKET;
	//SendString(XBeePacket, XBEE_PACKET_SIZE, XBEE);
	//fprintf (&__debug, "\nDECODE\n");
	switch(uart)
	{
		
		case XBEE:
		{
			XBeeSkeleton.syncChar1 = SYNC_CHAR_1;
			XBeeSkeleton.syncChar2 = SYNC_CHAR_2;
			XBeeSkeleton.type =   XBeePacket[0] ;
			XBeeSkeleton.source = XBeePacket[1];
			XBeeSkeleton.dest =   XBeePacket[2];
			XBeeSkeleton.data =                     ((int) XBeePacket[3]) << 24;
			XBeeSkeleton.data = XBeeSkeleton.data | ((int) XBeePacket[4]) << 16;
			XBeeSkeleton.data = XBeeSkeleton.data | ((int) XBeePacket[5]) << 8;
			XBeeSkeleton.data = XBeeSkeleton.data | ((int) XBeePacket[6]);
			
			validity = PACKET_FROM_XBEE;
		} break;
		
		case FM:
		{
			FMSkeleton.syncChar1 = SYNC_CHAR_1;
			FMSkeleton.syncChar2 = SYNC_CHAR_2;
			FMSkeleton.type = FMPacket[0] ;
			FMSkeleton.data =                   ((int) FMPacket[1]) << 24;
			FMSkeleton.data = FMSkeleton.data | ((int) FMPacket[2]) << 16;
			FMSkeleton.data = FMSkeleton.data | ((int) FMPacket[3]) << 8;
			FMSkeleton.data = FMSkeleton.data | ((int) FMPacket[4]);
			FMSkeleton.crc  =                   ((int) FMPacket[5]) << 8 ;
			FMSkeleton.crc  = FMSkeleton.crc  | ((int) FMPacket[6]);
			
			if (FMSkeleton.crc == calculatesCRC(FMPacket, FM_PACKET_SIZE-4)) //crc valid : FMSkeleton.crc == crc calculation du tonnerre
			{
				validity = PACKET_FROM_FM;
			}
			else
			{
				validity = NO_PACKET;
			}
		} break;
		
		case GSM:
		{
			
		} break;
		
	}
	
	return validity;
	
}

/** Send a frame from a source to a destination
 * Returns an integer : 0 if transmission done, 1 if an error occurs
 */
void sendPacket(ID_UART uart, unsigned char type, unsigned char source, unsigned char dest, int data)
{
	int repetition;

	switch(uart)
	{
		
		case XBEE:
		{
			unsigned char XBeePacketToSend[XBEE_PACKET_SIZE] ;
			
			XBeePacketToSend[0] = SYNC_CHAR_1 ;
			XBeePacketToSend[1] = SYNC_CHAR_2 ;
			XBeePacketToSend[2] = type ;
			XBeePacketToSend[3] = source ;
			XBeePacketToSend[4] = dest  ;
			XBeePacketToSend[5] = data >> 24 ;
			XBeePacketToSend[6] = data >> 16 ;
			XBeePacketToSend[7] = data >> 8 ;
			XBeePacketToSend[8] = data ;

			SendString(XBeePacketToSend, XBEE_PACKET_SIZE, XBEE);
		} break;
		
		case FM:
		{
			unsigned char FMPacketToSend[FM_PACKET_SIZE] ;
			unsigned short int crc;
			
			FMPacketToSend[0] = SYNC_CHAR_1 ;
			FMPacketToSend[1] = SYNC_CHAR_2 ;
			FMPacketToSend[2] = type ;
			FMPacketToSend[3] = data >> 24 ;
			FMPacketToSend[4] = data >> 16 ;
			FMPacketToSend[5] = data >> 8 ;
			FMPacketToSend[6] = data ;
			crc = calculatesCRC(&FMPacketToSend[2], FM_PACKET_SIZE-4); // No sync char neither crc in the calculation of the crc
			FMPacketToSend[7] = crc >> 8;
			FMPacketToSend[8] = crc;

			for (repetition = 0; repetition < FM_SEND_REPETITIONS; repetition++)
			{
				SendString(FMPacketToSend, FM_PACKET_SIZE, FM);
			}
			
		} break;
		
		case GSM:
		{
			
		} break;
		
	}
}
