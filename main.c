/*
 * Reseau de capteur version 7
 *
 *   Avant de vous exciter � taper du code partout, remplissez les champs suivants:
 *
 *	 Nom du bin�me: Les moustach'gracias
 *   Groupe : 0
 *   Quelle note esperez vous (honnetement) avoir ?: 15  
 *
 * Les clefs de compilation suivantes peuvent etre utilis�es:
 *         _LCD_DEBUG_: Permet de desactiver l'ecran LCD et les temporisations bloquantes en simul�
 */

#include "wireless_configuration.h"
#include "communication_protocol.h"

#include "global.h"

/*********************** LISEZ CA AVANT DE PASSER A LA SUITE *************************
 *
 * le scanf n'existe pas: vous l'avez imagin�, oubliez le !
 * printf ecrit sur l'ecran LCD
 * si vous devez faire du formatage de chaine, utilisez la forme sprintf
 * si vous devez faire de l'analyse de chaine, utilisez la forme sscanf (ou autre chose, genre strtok)
 * si vous ne savez pas utiliser sprintf et/ou sscanf et/ou strtok et/ou autres fonctions de string.h, demandez � Google !
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
	
	Noeud :
	si timeout : 		envoi message � base avec son id et un code d'erreur
	si r�ception � destination de la base : transfert
				
	Capteur :
	si �v�nement : 	envoi vers la base
	si timeout : 		envoi message de presence vers la base avec son id
	
	Base :
		quand re�oit :
			si message du capteur diff�rent du pr�c�dent :
				envoi par gsm
			si message r�seau:
				si message provenant du capteur : rien
				si message provenant d'un noeud : envoi par gsm du coeur ne fonctionnant plus
	
	RESTE DEV :
	- Config GSM
	- Envois GSM		
	RESTE DEBUG :
	- R�ussir � utiliser le CRC
	- SendPacket XBee : SIZE+2 sinon �a ne marche pas, pourquoi ?
	- �tre sur le bon PAN ID
	- N'afficher une erreur de r�seau que lorsqu'on est certain de sa source
*/
		
// Routing table
short int routingTable[NB_CORES] = ROUTING_TABLE;

// XBee Rx FSM state
int XBeeRxState = STATE_IDLE;

// FM Rx FSM state
int FMRxState = STATE_IDLE;

// Flag to indicate if a packet is received (move to another file later)
int packetReceived = NO_PACKET; 

// Value of the sensor : sending this value if it changes
int sensorValue = 1;

/*
 * int main (void)
 *
 * Point d'entr�e du programme
 *
 * Parametres:
 *     Entr�e: 
 *            aucun	
 *     Sortie:
 *            aucun
 *
 * Limitations:
 *     La fonction ne doit pas se terminer -> Boucle infinie
 */
int main (void)
{

	int cnt100Hz = 0;		// Counter incremented every 10ms
	int	cntReload = 0;	// Counter incremented at each theoretical reception of a network message from the sensor

	int tmpSensorValue = 1;

	/* Initialisation de la couche materielle */
	MACInit(); 

	/* Initialisation des media de communication */
	switch (TARGET)
	{
		case BASE:
			InitMessageLED();
			configureXBee(TURN_ON_RX_AND_TX);
			// configureGSM
			set_cursor(0,1);
			printf(" Bouton relache ");
			break;
		case NODE1:
		{
			InitMessageLED();
			configureXBee(TURN_ON_RX_AND_TX);
		} break;
		case NODE2:
		{
			InitMessageLED();
			configureXBee(TURN_ON_RX_AND_TX);
			configureFM(TURN_ON_RX);
		} break;
		case SENSOR:
		{
			GPIO_InitTypeDef GPIO_InitStruct;
			GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
			GPIO_Init(GPIOA, &GPIO_InitStruct);
			sensorValue = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);

			InitMessageLED();
			configureFM(TURN_ON_TX);
		} break;
	}
	
	if (TypeCarte == STANDARD)
	{
		//clearScreen();
	}

	/* Boucle principale, infinie */
	while (1)
	{
	
		if (packetReceived != NO_PACKET)
		{
			
			switch (TARGET)
			{
				case BASE:
					if ((packetReceived == PACKET_FROM_XBEE) && (XBeeSkeleton.dest == BASE))
					{

						cntReload++;

						switch (XBeeSkeleton.type)
						{
							
							case TYPE_DATA:
							{
								// Envoi GSM
								set_cursor(0,1);
								if (XBeeSkeleton.data != 0)
									printf(" Bouton relache ");
								else
									printf(" Bouton enfonce ");
							} break;
							
							case TYPE_NETWORK:
							{
								set_cursor(0,0);
								printf("   Network OK   ");
							} break;
							
							case TYPE_ERROR:
							{
								// Envoi GSM
								set_cursor(0,0);
								switch (XBeeSkeleton.data)
								{
									case SENSOR:
										printf("Err : Sensor off");
									break;
									
									case NODE2:
										printf("Err : Node 2 off");
									break;
								}
							} break;
							
						}
					}
					break;
				
				case NODE1:
					if ((packetReceived == PACKET_FROM_XBEE) && (XBeeSkeleton.dest == NODE1)) // Transfers to the base
					{
						if (XBeeSkeleton.type == TYPE_DATA)
						{
							sensorValue = XBeeSkeleton.data;
						}				
						cntReload++;						
						sendPacket(XBEE, XBeeSkeleton.type, XBeeSkeleton.num_seq, NODE1, BASE, XBeeSkeleton.count++, XBeeSkeleton.data);
					}
					break;
				
				case NODE2:
					if (packetReceived == PACKET_FROM_FM) // Transfers to the next node (indicates if the CRC was correct)
					{
						if (FMSkeleton.type == TYPE_NETWORK)
						{
							if (cntReload++ == 0)
							{
								sendPacket(XBEE, FMSkeleton.type, 0, NODE2, NODE1, 0, FMSkeleton.data);
							}
						}
						else if (FMSkeleton.type == TYPE_DATA)
						{
							tmpSensorValue = FMSkeleton.data;
							if (tmpSensorValue != sensorValue) // Send only if the value has changed
							{
								sensorValue = tmpSensorValue;
								sendPacket(XBEE, FMSkeleton.type, 0, NODE2, NODE1, 0, FMSkeleton.data);
							}
						}						

					}
					break;
				
				case SENSOR:
					break;
			}			

			packetReceived = NO_PACKET;
		}
		
		// Alert button polling
		if (TARGET == SENSOR) 
		{
			
			// Send the new sensor value if it has changed
			tmpSensorValue = averageSensorValue(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0));
			if (tmpSensorValue != sensorValue)
			{
				sensorValue = tmpSensorValue;
				sendPacket(FM, TYPE_DATA, 0, 0, 0, 0, sensorValue);
			}
			
		}
		
		// Displays the value of the message on the green LED (Olimex Cards only)
		SetMessageLED( sensorValue );
		
		// Network checking
		if	( TIME10msExpired() )	// relaunch its own timer
		{
			cnt100Hz++;
			
			// Sensor : sends messages every 700ms
			if ( cnt100Hz >= 70  && TARGET == SENSOR )	// Every 0,70s
			{
				cnt100Hz = 0;
				sendPacket(FM, TYPE_NETWORK, 0, SENSOR, BASE, 0, 0);
			}

			// Node : if no message received after 1,5s, sends an error
			if ( cnt100Hz >= 150  && TARGET != SENSOR && TARGET != BASE )	// Every 1,50s
			{
				cnt100Hz = 0;
				if (cntReload == 0)
				{
					if (TARGET == NODE2)
						sendPacket(XBEE, TYPE_ERROR, 0, NODE2, NODE1, 0, SENSOR);
					if (TARGET == NODE1)
						sendPacket(XBEE, TYPE_ERROR, 0, NODE1, BASE, 0, NODE2);
				}
				else
				{
					cntReload = 0;
				}
			}
			
			// Base : if no message received after 1,5s, rise an error
			if ( cnt100Hz >= 150  && TARGET == BASE )	// Every 1,50s
			{
				cnt100Hz = 0;
				if (cntReload == 0)
				{
					set_cursor(0,0);
					printf("Err : Node 1 off");
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
 * Fonction appel�e par les interruptions UART � chaque octet recu
 *
 * Parametres:
 *     Entr�e: 
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
				/*else
				{
					set_cursor(0,1);
					printf("Wrong Rx : %c",d);
				}*/
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
		/*
		 * TODO: rajoutez le traitement a faire lors de la reception via GSM 
		 */
		break;
	}
}							

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
