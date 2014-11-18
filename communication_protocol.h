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

#define XBEE_PACKET_SIZE			10 // Bytes
#define FM_PACKET_SIZE				10 // Bytes

/* Add a byte to the packet being filled.
 * Returns 0 if the packet is not fully filled, 1 either.
 */
int addReceivedByte(char byte, ID_UART uart);

/** Translate the bytes of the filled packet.
 * Returns a structure containing all the fields.
 */
void test(ID_UART uart);


#endif /* __COMMUNICATION_PROTOCOL__ */
