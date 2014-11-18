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
typedef struct
{
	int id; // 2 bytes
	int payload; // 4 bytes
} XBeeFrame;

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
void test(ID_UART uart)
{
	
	SendString(XBeePacket, XBEE_PACKET_SIZE, XBEE);
	
	switch(uart)
	{
		
		case XBEE:
		{

		} break;
		
		case FM:
		{
			
		} break;
		
		case GSM:
		{
			
		} break;
		
	}
	
}
