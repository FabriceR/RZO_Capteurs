/*
 * Développement d'une interface en C pour configurer nos modules
 *
 *   XBee, FM, GSM
 *
 * Les enumerations sont issues de mac.h
 *
 */
 
#include "wireless_configuration.h"
#include "mac.h"

#ifndef __COMMUNICATION_PROTOCOL__
#define __COMMUNICATION_PROTOCOL__

#define XBEE_PACKET_SIZE			13 // Bytes
#define FM_PACKET_SIZE				4 // Bytes


typedef struct
{
	unsigned char type ; // 1 byte
	unsigned char num_seq ; // 1 byte
	short int source ; // 2 bytes
	short int dest ; // 2 bytes
	unsigned char count ; // 1 byte
	long int data ; // 4 bytes
} XBeeFrame;

/* Add a byte to the packet being filled.
 * Returns 0 if the packet is not fully filled, 1 either.
 */
int addReceivedByte(char byte, ID_UART uart);

/** Translate the bytes of the filled packet.
 * Returns a structure containing all the fields.
 */
void decodePacket(ID_UART uart);

/** Send a frame from a source to a destination
 * Returns an integer : 0 if transmission done, 1 if an error occurs
 */
void sendPacket(ID_UART uart, unsigned char type, unsigned char num_seq, short int source, short int dest, unsigned char count, long int data);

#endif /* __COMMUNICATION_PROTOCOL__ */
