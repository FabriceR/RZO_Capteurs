/*
 * Constantes globales au projet
 *
 *   Adresses, IDs, ...
 *
 */

#include "mac.h" 	  /* <-- avec ca, vous avez accez au niveau materiel  */

#include <stdio.h>    /* <-- avec ca, vous avez acces à sprintf et sscanf */
#include <string.h>	  /* <-- et avec ca, vous avez acces à toutes les fonctions de gestion des chaines */

#include "Retarget.h"


#ifndef __GLOBAL__
#define __GLOBAL__

#define	NB_SENSOR_VALUES_AVG	750
#define FM_SEND_REPETITIONS		15 // Number of same sends for one message

typedef enum
{
	BASE = 'B',
	NODE1 = '1',
	NODE2 = '2',
	SENSOR = 'S',
	// Number of cores
	NB_CORES = 4,
} CORES;

///////////////////////////////////
// CHANGE HERE THE CORE TO FLASH //
///////////////////////////////////
#define TARGET	BASE
///////////////////////////////////
// CHANGE HERE THE CORE TO FLASH //
///////////////////////////////////

#define ROUTING_TABLE	{ 0x1001, 0x2001, 0x2002, 0x3001 }
extern short int routingTable[NB_CORES];

int averageSensorValue(int newValue);
void InitMessageLED( void );
void SetMessageLED( int value );

#endif /* __GLOBAL__ */
