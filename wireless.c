/*
 * Développement d'une interface en C pour configurer nos modules
 *
 *   XBee, FM, GSM
 *
 * Les enumerations sont issues de mac.h
 *
 */
 
#include "wireless.h"

// Indicates if a OK has been received
int OKReceived = 0;

/** Set the OKReceived flag to true.
 */
void setOKReceived( void )
{
	OKReceived = 1;
}

/** Configure FM module and STM interrupts
 * direction comes from the enumeration FM_DIRECTION in wireless.h
 * Achtung : Do not try to send right after a receive interrupt, it does not work
 * Debug may be done using the printf method to write on the LCD screen.
 */
int configureFM( int direction )
{
	int out = SUCCESS;
	
	// Closing properly last configuration
	DesactiverITRXFM();
	TurnOffFM();
	
	// Opening a clean configuration
	switch (direction)
	{
		
		case TURN_ON_RX:
		{
			TurnOnRXFM();
			ActiverITRXFM();
		} break;
	
		case TURN_ON_TX:
		{
			TurnOnTXFM();
		} break;
		
		default:
		{
			out = ERROR;
		}
		
	} // Fin switch : direction ?
	
	return out;
}


/** Configure XBee module and STM interrupts
 * At opening, the modules are all configured to work with a given panid
 * in order to not disturb other communication.
 */
int configureXBee( int direction )
{
	int out = SUCCESS;

	
	// Closing properly last configuration
	DesactiverITRXXBEE();
	
	// Opening a clean configuration
	switch (direction)
	{
	
		case TURN_ON_RX:
		{
			ActiverITRXXBEE();
		} break;
		
		case TURN_ON_TX:
		{
			// NOP
		} break;
		
		case TURN_ON_RX_AND_TX:
		{
			ActiverITRXXBEE();
		} break;
		
		default:
		{
			out = ERROR;
		}

	} // Fin switch : direction
	
	// Configure XBee module
	if (out == SUCCESS)
	{
		TIMEWaitxms(500);

		// Open command prompt
		out = sendCommand(PPP);
		TIMEWaitxms(500);
		if (out == SUCCESS)
			out = sendCommand(ATID);
		if (out == SUCCESS)
			out = sendCommand(ATWR);
		if (out == SUCCESS)
			out = sendCommand(ATCN);
	}
	
	return out;
}
	
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
void SendString(unsigned char * str, int length, ID_UART uart)
{
	int id;
	for (id = 0; id < length; id++)
	{
		SendData((unsigned char)str[id], uart);
	}
}

/** Nothing but a clean of the LCD screen
 */
void clearScreen( void )
{
	if (TypeCarte == STANDARD)
	{
		set_cursor(0,0);
		printf("                ");
		set_cursor(0,1);
		printf("                ");
		set_cursor(0,0);
	}
}
	

/** Send commands to configuration purposes
 */
int sendCommand( int command )
{
	int out = ERROR;
	int runs = 2;
	unsigned char cmd[20];

	while ((out == ERROR) && (runs != 0))
	{
		OKReceived = 0;
		
		switch (command)
		{
			
			case PPP:
				sprintf((char *)cmd, "+++");
				SendString(cmd, 3, XBEE);
				TIMEWaitxms(800);
			break;
			
			case ATID:
				sprintf((char *)cmd, "ATID%d\r", XBEE_PANID);
				SendString(cmd, 9, XBEE);
				TIMEWaitxms(200);
			break;
			
			case ATWR:
				sprintf((char *)cmd, "ATWR\r");
				SendString(cmd, 5, XBEE);
				TIMEWaitxms(200);
			break;
			
			case ATCN:
				sprintf((char *)cmd, "ATCN\r");
				SendString(cmd, 5, XBEE);
				TIMEWaitxms(200);
			break;
			
		}
		
		// Clean screen for future display
		clearScreen();

		// Verdict
		if (OKReceived)
		{

			OKReceived = 0;
			out = SUCCESS;

			if (TypeCarte == STANDARD)
			{
				switch (command)
				{
					
					case PPP:
						printf("     +++ OK     ");
					break;
					
					case ATID:
						printf("    ATID OK     ");
					break;
					
					case ATWR:
						printf("    ATWR OK     ");
					break;
					
					case ATCN:
						printf("    ATCN OK     ");
					break;
					
				}
			}
			
		}
		else
		{
			if (TypeCarte == STANDARD)
			{
				printf(" NO XBEE ANSWER ");
				set_cursor(0,1);
				printf("... Waiting ... ");
			}
			TIMEWaitxms(500);
			out = ERROR;
			runs--;	// While loop run max 2 times.
		}
		
	}
	
	if ((out == ERROR) && (TypeCarte == STANDARD))
	{
		clearScreen();
		printf(" !! FAILURE !!  ");
	}
	
	return out;
}

