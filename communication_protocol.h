/*
 * Développement d'une interface en C pour configurer nos modules
 *
 *   XBee, FM, GSM
 *
 * Les enumerations sont issues de mac.h
 *
 */

#include "global.h"
#include "wireless_configuration.h"

#ifndef __COMMUNICATION_PROTOCOL__
#define __COMMUNICATION_PROTOCOL__


#define XBEE_PACKET_SIZE			13 // Bytes
#define FM_PACKET_SIZE				9 // Bytes

typedef enum
{
	TYPE_DATA = 0,
	TYPE_NETWORK,
	TYPE_ERROR,
} TypeOfMessage;

typedef enum
{
	NO_PACKET = 0,
	PACKET_FROM_FM,
	PACKET_FROM_XBEE,
	PACKET_FROM_GSM,
} OriginOfPacket;

// FM packet structure
typedef struct
{
	unsigned char syncChar1;	// 1 byte
	unsigned char syncChar2;	// 1 byte
	unsigned char type ; 			// 1 byte
	int data ; 								// 4 bytes
	unsigned short int crc ;	// 2 bytes
} FMFrame;

extern FMFrame FMSkeleton ;

// XBee packet structure
typedef struct
{
	unsigned char syncChar1;	// 1 byte
	unsigned char syncChar2;	// 1 byte
	unsigned char type ; 			// 1 byte
	unsigned char num_seq ; 	// 1 byte
	unsigned short int source ; // 2 bytes
	unsigned short int dest ; 	// 2 bytes
	unsigned char count ; 		// 1 byte
	int data ; 								// 4 bytes
} XBeeFrame;

extern XBeeFrame XBeeSkeleton ;

/* Add a byte to the packet being filled.
 * Returns 0 if the packet is not fully filled, 1 either.
 */
int addReceivedByte(char byte, ID_UART uart);

/** Translate the bytes of the filled packet.
 * Returns the validity of the packet and its origin
 */
int decodePacket(ID_UART uart);

/** Send a frame from a source to a destination
 * Returns an integer : 0 if transmission done, 1 if an error occurs
 */
void sendPacket(ID_UART uart, unsigned char type, unsigned char num_seq, unsigned short int source, unsigned short int dest, unsigned char count, long int data);
 
#endif /* __COMMUNICATION_PROTOCOL__ */
