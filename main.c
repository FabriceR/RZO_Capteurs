/*
 * Reseau de capteur version 7
 *
 *   Avant de vous exciter à taper du code partout, remplissez les champs suivants:
 *
 *	 Nom du binôme: Les moustach'gracias
 *   Groupe : 0
 *   Quelle note esperez vous (honnetement) avoir ?: 15  
 *
 * Les clefs de compilation suivantes peuvent etre utilisées:
 *         _LCD_DEBUG_: Permet de desactiver l'ecran LCD et les temporisations bloquantes en simulé
 */

#include "wireless_configuration.h"
#include "communication_protocol.h"

#include "global.h"

/*********************** LISEZ CA AVANT DE PASSER A LA SUITE *************************
 *
 * le scanf n'existe pas: vous l'avez imaginé, oubliez le !
 * printf ecrit sur l'ecran LCD
 * si vous devez faire du formatage de chaine, utilisez la forme sprintf
 * si vous devez faire de l'analyse de chaine, utilisez la forme sscanf (ou autre chose, genre strtok)
 * si vous ne savez pas utiliser sprintf et/ou sscanf et/ou strtok et/ou autres fonctions de string.h, demandez à Google !
 *
 * Bonne chance
 *
 *************************************************************************************/
/* printf vers le debugger Keil (seulement avec cible reelle, pas en simu) (sonde sur port SW, pas JTAG) :
 * pour l'activer :
 * 	Target Options --> onglet Debug --> bouton Settings --> onglet Trace --> Trace Enable
 * pour visualiser les messages en mode debug :
 *	View --> Serial Windows --> Debug (printf) Viewer
 * fprintf (&__debug, "Debug ON\n");
 */
/** PROTOCOLE
	
	Capteur :
		Si événement : 	Envoi vers la base de l'événement
		Si timeout : 		Envoi message de presence vers la base avec son id
	
	Noeud :
		Si réception : 	Transfère le paquet au noeud suivant et réarme le timer d'attente
		Si timeout : 		Envoi message d'erreur avec l'identifiant du noeud ne fonctionnant plus
	
	Base :
		Si réception :
			Data :				Si message du capteur différent du précédent : Envoi par gsm
			Réseau :			Réarme son timer d'attente
			Erreur :			Envoi par gsm du coeur ne fonctionnant plus
		Si timeout : 		Envoi par gsm du coeur ne fonctionnant plus
*/
		
/*	TODO : 
	- Envoi du bon texto après une période de confirmation de deux ou trois erreurs de réseau
	- Envoi d'un seul putain de texto
	- Affichage à régler ?
*/

// XBee Rx FSM state
int XBeeRxState = STATE_IDLE;

// FM Rx FSM state
int FMRxState = STATE_IDLE;

// Flag to indicate if a packet is received (move to another file later)
int packetReceived = NO_PACKET; 

// Message to be sent through the GSM
unsigned char gsmMessage[50];
int gsmLength ;

/*
 * int main (void)
 *
 * Point d'entrée du programme
 *
 * Parametres:
 *     Entrée: 
 *            aucun	
 *     Sortie:
 *            aucun
 *
 * Limitations:
 *     La fonction ne doit pas se terminer -> Boucle infinie
 */
int main (void)
{

	int cnt100Hz = 0;			// Counter incremented every 10ms
	int	cntReload = 0;		// Counter incremented at each theoretical reception of a network message from the sensor

	int sensorValue = 1; 	// Value of the sensor : sending this value if it changes
	int tmpSensorValue = 1;
	
	/* Hadware layer init */
	MACInit(); 

	/* Inits for each possible node (Sensor, Node2, Node1, Base) */
	switch (TARGET)
	{
		case BASE:
		{
			// configureGSM
			InitMessageLED();										// Turn off all LEDs
			configureXBee(TURN_ON_RX_AND_TX);		// Init XBee
			set_cursor(0,1);										// Displays a std message
			printf(" Bouton relache ");
		}	break;
		case NODE1:
		{
			InitMessageLED();										// Init LED to display if the button is pushed
			configureXBee(TURN_ON_RX_AND_TX);		// Init XBee
		} break;
		case NODE2:
		{
			InitMessageLED();										// Init LED to display if the button is pushed
			configureXBee(TURN_ON_RX_AND_TX);		// Init XBee
			configureFM(TURN_ON_RX);						// Init FM (Rx)
		} break;
		case SENSOR:
		{
			GPIO_InitTypeDef GPIO_InitStruct;		// Init GPIO for polling the value of the button
			GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
			GPIO_Init(GPIOA, &GPIO_InitStruct);
			sensorValue = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);

			InitMessageLED();										// Init LED to display if the button is pushed
			configureFM(TURN_ON_TX);						// Init FM (Tx)
		} break;
	}

	/* Infinite loop */
	while (1)
	{
	
		/* On reception of a packet */
		if (packetReceived != NO_PACKET)
		{
			
			switch (TARGET)
			{
				
				/* Base : Concentrates all the messages */
				case BASE:
					if ((packetReceived == PACKET_FROM_XBEE) && (XBeeSkeleton.dest == BASE))
					{

						cntReload++;														// Reload the timeout counter (to get if a node is disconnected)

						switch (XBeeSkeleton.type)
						{
							
							case TYPE_DATA:
							{
								
								set_cursor(0,1);
								if (XBeeSkeleton.data != 0)
								{
									printf(" Bouton relache ");
								}									
								else
								{
									printf(" Bouton enfonce ");

									// Send a message through GSM									
									set_cursor(0,0);
									printf("Env SMS Presence");
									strcpy((char *)gsmMessage, "Attention : présence détectée !");
									gsmLength = strlen((const char *) gsmMessage);
									sendGSM((unsigned char *) PHONE_NUMBER_FWAB, gsmMessage, gsmLength);
								}
								sensorValue = XBeeSkeleton.data;
							} break;
							
							case TYPE_NETWORK:
							{
								set_cursor(0,0);
								printf("   Network OK   ");
								
								// Update sensor value if needed
								set_cursor(0,1);
								if (XBeeSkeleton.data != 0)
									printf(" Bouton relache ");
								else
									printf(" Bouton enfonce ");
								sensorValue = XBeeSkeleton.data;
							} break;
							
							case TYPE_ERROR:
							{
								set_cursor(0,0);
								switch (XBeeSkeleton.data)
								{
									case SENSOR:
										//printf("Err : Sensor off");
																		
										//set_cursor(0,1);
										printf(" SMS Sensor off ");
										strcpy((char *)gsmMessage, "Attention : Sensor off !");
										gsmLength = strlen((const char *) gsmMessage);
										sendGSM((unsigned char *) PHONE_NUMBER_MICH, gsmMessage, gsmLength);
									break;
									
									case NODE2:
										//printf("Err : Node 2 off");
									
										//set_cursor(0,1);
										printf(" SMS Node 2 off ");
										strcpy((char *)gsmMessage, "Attention : Node 2 off !");
										gsmLength = strlen((const char *) gsmMessage);
										sendGSM((unsigned char *) PHONE_NUMBER_MICH, gsmMessage, gsmLength);
									break;
								}
								
							} break;
							
						}
					}
					break;
				
				/* Node 1 : Transfers the packets to the base */
				case NODE1:
					if ((packetReceived == PACKET_FROM_XBEE) && (XBeeSkeleton.dest == NODE1)) // Transfers to the base
					{
						if (XBeeSkeleton.type != TYPE_ERROR)
						{
							sensorValue = XBeeSkeleton.data;		// Saves the current value of the sensor
						}				
						cntReload++;													// Reload the timeout counter (to get if a node is disconnected)						
						sendPacket(XBEE, XBeeSkeleton.type, NODE1, BASE, XBeeSkeleton.data);
					}
					break;
				
				/* Node 1 : Get the packets from FM and transfers the packets to the base */
				case NODE2:
					if (packetReceived == PACKET_FROM_FM) 	// Transfers to the next node (indicates if the CRC was correct)
					{
						
						if ( (FMSkeleton.type == TYPE_NETWORK && cntReload == 0) // Send only if needed (many FM sends, only one transmission through XBee
							|| (FMSkeleton.type == TYPE_DATA    && FMSkeleton.data != sensorValue)) // Send only if the value has changed
						{
							sendPacket(XBEE, FMSkeleton.type, NODE2, NODE1, FMSkeleton.data);
							sensorValue = FMSkeleton.data;
						}				

						cntReload++;													// Reload the timeout counter (to get if a node is disconnected)

					}
					break;
				
				case SENSOR:
					break;
			}			

			packetReceived = NO_PACKET;
		}
		
		/* Alert button polling */
		if (TARGET == SENSOR) 
		{
			
			// Send the new sensor value if it has changed
			tmpSensorValue = averageSensorValue(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0));
			if (tmpSensorValue != sensorValue)
			{
				sensorValue = tmpSensorValue;
				sendPacket(FM, TYPE_DATA, 0, 0, sensorValue);
			}
			
		}
		
		/* Displays the value of the message on the green LED (Olimex board) or on the yellow 15th LED (Keil board) */
		SetMessageLED( sensorValue );
		
		/* Network checking */
		if	( TIME10msExpired() )	// relaunch its own timer
		{
			cnt100Hz++;
			
			// Sensor : sends messages regularly
			if ( cnt100Hz >= CYCLES_BETWEEN_NETWORK_MESSAGES  && TARGET == SENSOR )
			{
				cnt100Hz = 0;
				sendPacket(FM, TYPE_NETWORK, SENSOR, BASE, sensorValue);
			}

			// Node : if no message received after a given time, sends an error
			if ( (cnt100Hz >= CYCLES_BEFORE_ALARM_NODE2 && TARGET == NODE2 )
				|| (cnt100Hz >= CYCLES_BEFORE_ALARM_NODE1 && TARGET == NODE1 ))
			{
				cnt100Hz = 0;
				if (cntReload == 0)
				{
					if (TARGET == NODE2)
						sendPacket(XBEE, TYPE_ERROR, NODE2, NODE1, SENSOR);
					if (TARGET == NODE1)
						sendPacket(XBEE, TYPE_ERROR, NODE1, BASE, NODE2);
				}
				else
				{
					cntReload = 0;
				}
			}
			
			// Base : if no message received at timeout : rise an error
			if ( cnt100Hz >= CYCLES_BEFORE_ALARM_BASE && TARGET == BASE )
			{
				cnt100Hz = 0;
				if (cntReload == 0)
				{
					set_cursor(0,0);
					//printf("Err : Node 1 off");
															
					//set_cursor(0,1);
					printf(" SMS Node 1 off ");
					strcpy((char *)gsmMessage, "Attention : Node 1 off !");
					gsmLength = strlen((const char *) gsmMessage);
					sendGSM((unsigned char *) PHONE_NUMBER_MICH, gsmMessage, gsmLength);
				}
				else
				{
					cntReload = 0;
				}
			}
			
		} // if TIME10msExpired
		
	} // while (1)
}

/*
 * void DataReceived (unsigned char d, ID_UART uart)
 *
 * Fonction appelée par les interruptions UART à chaque octet recu
 *
 * Parametres:
 *     Entrée: 
 *            d (unsigned char): octet recu
 *            uart (ID_UART) : Identifiant de l'UART ayant recu l'octet	
 *     Sortie:
 *            aucun
 *
 * Limitations:
 *     Attention ! Vous etes sous interruption ici ... Pas de boucle infinie, et n'y restez pas des heures.
 */
void DataReceived (unsigned char d, ID_UART uart)
{
	switch (uart)
	{
	case XBEE:

		switch (XBeeRxState)
		{
		
			case 	STATE_IDLE:
			{
				if (d == SYNC_CHAR_1)
					XBeeRxState = STATE_SYNC;
				else if (d == CONF_CHAR_1)
					XBeeRxState = STATE_CONF;
			} break;
			
			case STATE_SYNC:
			{
				if (d == SYNC_CHAR_2)
					XBeeRxState = STATE_SAVING;
				else
					XBeeRxState = STATE_IDLE;
			} break;
			
			case STATE_CONF:
			{
				if (d == CONF_CHAR_2)
					setOKReceived();
				XBeeRxState = STATE_IDLE;
			} break;
			
			case STATE_SAVING:
			{
				if (addReceivedByte(d, XBEE))
				{
					packetReceived = decodePacket( uart );
					XBeeRxState = STATE_IDLE;
				}
			} break;
			
		}
	
		break;
	case FM:
		switch (FMRxState)
		{
		
			case 	STATE_IDLE:
			{
				if (d == SYNC_CHAR_1)
					FMRxState = STATE_SYNC;
			} break;
			
			case STATE_SYNC:
			{
				if (d == SYNC_CHAR_2)
					FMRxState = STATE_SAVING;
				else
					FMRxState = STATE_IDLE;
			} break;
			
			case STATE_SAVING:
			{
				if (addReceivedByte(d, FM))
				{
					packetReceived = decodePacket( uart );
					FMRxState = STATE_IDLE;
				}
			} break;
			
		}
		
		break;
	case GSM:
		// NOP
		break;
	}
}							

/** Save many values of the sensor and checks if all the data stored is the same
 *  Easy way to be sure the button is not shaking anymore before sending any message
 */
int averageSensorValue(int newValue)
{
	static int values[NB_SENSOR_VALUES_AVG];
	static int idInValues = 0;
	int i = 0;
	int out = 1;
	
	// Add the new value in the array and checks the validity of the next id
	values[idInValues] = newValue;
	if (++idInValues == NB_SENSOR_VALUES_AVG)
	{
		idInValues = 0;
	}
	
	// Checks if the array is full of the same value
	for (i = 1; i < NB_SENSOR_VALUES_AVG; i++)
	{
		if (values[i] != values[0])
		{
			out = 1;
			break;
		}
		else
		{
			out = values[0];
		}
	}
	
	return out;
	
}

/** Init of the GPIO using the LEDs
 *  Init depends on the board used (Olimex or Keil)
 */
void InitMessageLED( void )
{
	GPIO_InitTypeDef GPIO_InitStruct;
		
	if (TARGET == BASE)
	{
		// B 15 on Keil board
		GPIO_StructInit(&GPIO_InitStruct);
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
		
		GPIO_DeInit(GPIOB);
		GPIO_Init(GPIOB, &GPIO_InitStruct);

		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET);
		
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET);
		
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET);
		
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
		
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		GPIO_WriteBit(GPIOB, GPIO_Pin_11, Bit_RESET);
		
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_RESET);
		
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		GPIO_WriteBit(GPIOB, GPIO_Pin_9, Bit_RESET);
		
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_RESET);
	}
	else
	{
		// C 12 on Olimex board
		GPIO_StructInit(&GPIO_InitStruct);
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
		
		GPIO_DeInit(GPIOC);
		GPIO_Init(GPIOC, &GPIO_InitStruct);

		GPIO_WriteBit(GPIOC, GPIO_Pin_12, Bit_SET);
	}

}

/** Set the green LED on Olimex boards or the yellow LED on Keil board
 *  Manage the active-high/active-low functionalities
 */
void SetMessageLED( int value )
{
	if (value == 0)
	{
		if (TARGET == BASE)
			GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_SET);
		else
			GPIO_WriteBit(GPIOC, GPIO_Pin_12, Bit_RESET);
	}
	else
	{
		if (TARGET == BASE)
			GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET);
		else
			GPIO_WriteBit(GPIOC, GPIO_Pin_12, Bit_SET);
	}
}
