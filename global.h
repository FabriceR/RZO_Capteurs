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
#define FM_SEND_REPETITIONS		5 // Number of same sends for one message

#define CYCLES_BETWEEN_NETWORK_MESSAGES	70	// Cycles of 10ms
#define CYCLES_BEFORE_ALARM_NODE2	(CYCLES_BETWEEN_NETWORK_MESSAGES * 2 + 10)
#define CYCLES_BEFORE_ALARM_NODE1	(CYCLES_BETWEEN_NETWORK_MESSAGES * 3 + 10)
#define CYCLES_BEFORE_ALARM_BASE	(CYCLES_BETWEEN_NETWORK_MESSAGES * 4 + 10)

#define PHONE_NUMBER_FWAB	 		("0630879262")
#define PHONE_NUMBER_MICH	 		("0620533522")


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

/** Save many values of the sensor and checks if all the data stored is the same
 *  Easy way to be sure the button is not shaking anymore before sending any message
 */
int averageSensorValue(int newValue);

/** Init of the GPIO using the LEDs
 *  Init depends on the board used (Olimex or Keil)
 */
void InitMessageLED( void );

/** Set the green LED on Olimex boards or the yellow LED on Keil board
 *  Manage the active-high/active-low functionalities
 */
void SetMessageLED( int value );

#endif /* __GLOBAL__ */
