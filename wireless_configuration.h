/*
 * Développement d'une interface en C pour configurer nos modules
 *
 *   XBee, FM, GSM
 *
 * Les enumerations sont issues de mac.h
 *
 */
 
#ifndef __WIRELESS_CONFIGURATION__
#define __WIRELESS_CONFIGURATION__

#include <stdio.h>    /* sprintf et sscanf */
#include <string.h>	  /* gestion des chaines */
#include "global.h"

#define 	XBEE_PANID		3615
#define		SYNC_CHAR_1		'F'
#define		SYNC_CHAR_2 	'M'
#define		CONF_CHAR_1 	'O'
#define		CONF_CHAR_2 	'K'

typedef enum
{
	TURN_ON_RX = 0,
	TURN_ON_TX,
	TURN_ON_RX_AND_TX,
} FM_DIRECTION;

typedef enum
{
	STATE_IDLE = 0,	// Wait for a character
	STATE_SYNC,			// First sync character received, wait for the second
	STATE_CONF,			// First conf character received, wait for the second
	STATE_SAVING,		// Second sync character received, saving data (size known)
} RX_STATES;

typedef enum
{
	PPP = 0,				// Message +++
	ATID,						// Configuration ATID
	ATWR,						// Ecriture dans la flash ATWR
	ATCN,						// Fin de la configuration ATCN
} PROMPT_COMMANDS; // Type de message attendu



/** Set the OKReceived flag to true.
 */
void setOKReceived( void );

/** Configure FM module and STM interrupts
 * direction comes from the enumeration FM_DIRECTION in wireless.h
 * Achtung : Do not try to send right after a receive interrupt, it does not work
 * Debug may be done using the printf method to write on the LCD screen.
 */
int configureFM( int direction );


/** Configure XBee module and STM interrupts
 * At opening, the modules are all configured to work with a given panid
 * in order to not disturb other communication.
 */
int configureXBee( int direction );


/*
 * void SendString(unsigned char * str, int length, ID_UART uart) 
 *
 * Envoi d'une chaîne de caractères sur l'UART indiquée
 *
 * Parametres:
 *     Entrée:
 *	          str (unsigned char *): chaîne à envoyer
 *						length (int): longueur de la chaîne
 *            uart (ID_UART): identifiant de la liaison serie	
 *     Sortie:
 *            rien
 *
 * Limitations:
 *     Attention, fonction bloquante le temps de l'envoi de la chaîne.
 */
void SendString( unsigned char * str, int length, ID_UART uart );

/** Nothing but a clear of the LCD screen
 */
void clearScreen( void );

/** Send commands to configuration purposes
 */
int sendCommand( int command );

/** Calculates a CRC on 2 bytes to be added in the frame sent in order to check the validity of the frame
 * Uses : UpdateCRC of mac.c
 * Returns : short int CRC
 * In : int size of the data, unsigned char * pointer to the first byte of the data
 */
unsigned short int calculatesCRC(unsigned char * data , int sizeOfData);

#endif /* __WIRELESS_CONFIGURATION__ */
