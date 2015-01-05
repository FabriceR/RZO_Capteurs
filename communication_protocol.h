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


#define XBEE_PACKET_SIZE			9 // Bytes
#define FM_PACKET_SIZE				9 // Bytes
#define GSM_1ST_MSG_SIZE			21 // Bytes
#define GSM_POSITION_NUMBER		9 // Bytes

typedef enum
{
	TYPE_DATA = 'D',
	TYPE_NETWORK = 'N',
	TYPE_ERROR = 'E',
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
	unsigned char source ; 		// 1 bytes
	unsigned char dest ; 			// 1 bytes
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
 */
void sendPacket(ID_UART uart, unsigned char type, unsigned char source, unsigned char dest, int data);

/** Send a message using GSM 
 */
void sendGSM(unsigned char * number, unsigned char * message, unsigned char length);

#endif /* __COMMUNICATION_PROTOCOL__ */
