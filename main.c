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

#include "mac.h" 	  /* <-- avec ca, vous avez accez au niveau materiel  */

#include <stdio.h>    /* <-- avec ca, vous avez acces à sprintf et sscanf */
#include <string.h>	  /* <-- et avec ca, vous avez acces à toutes les fonctions de gestion des chaines */

#include "Retarget.h"

#include "wireless.h"

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

 // Xbee Rx FSM state
int XBeeRxState = STATE_IDLE;
 
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
// variables pour la demo timer+LED
// int cnt100Hz = 0, cnt1Hz = 0;	// compteurs
// int blink = 15;			// rapport cyclique LED (sur 100)

	/* Initialisation de la couche materielle */
	MACInit(); 

	/* Initialisation des media de communication */
	// configureFM(TURN_ON_RX);
	configureXBee(TURN_ON_RX_AND_TX);
	
	if (TypeCarte == STANDARD)
	{
		//clearScreen();
	}

	/* printf vers le debugger Keil (seulement avec cible reelle, pas en simu) (sonde sur port SW, pas JTAG) :
	 * pour l'activer :
	 * 	Target Options --> onglet Debug --> bouton Settings --> onglet Trace --> Trace Enable
	 * pour visualiser les messages en mode debug :
	 *	View --> Serial Windows --> Debug (printf) Viewer
	 */
	fprintf (&__debug, "Un coucou du debug \n");
	
	/* Boucle principale, infinie */
	while (1)
	{
		
		SendString((unsigned char *)"RESERVED PAN ID !\r", 18, XBEE);
		TIMEWaitxms(1000);
	
		/* Fabrice ne regarde pas la suite c'est caca */
		/*if	( TIME10msExpired() )	// N.B. cette fonction a un effet de bord : elle re-arme son propre timer 
			{
			if	( ++cnt100Hz >= 100 )			// une fois par seconde
				{
				cnt100Hz = 0; cnt1Hz += 1;
				if	( TypeCarte == STANDARD )
					{
					set_cursor(0,1);
					printf("%6u ", cnt1Hz );
					}
				else	{	// voir plus haut activation du printf debug
					fprintf (&__debug, "%6u \n", cnt1Hz );
					}
				}
			// faire clignoter 1 LED avec une résolution de 10 ms
			if	(TypeCarte == GATEWAY)			// toutes les 10 ms
				{				
				if	( cnt100Hz < blink )		// blink 1Hz
					GPIOC->BRR  = GPIO_Pin_12;	// l'unique LED de la carte Olimex
				else	GPIOC->BSRR = GPIO_Pin_12;	// active a l'etat bas
				}
			else	{
				if	( cnt100Hz < blink )		// blink 1Hz
					GPIOB->BSRR = GPIO_Pin_15;	// la LED de droite de la carte Keil
				else	GPIOB->BRR  = GPIO_Pin_15;	// active a l'etat haut
				}
			} // if TIME10msExpired() */
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
				else
				{
					set_cursor(0,1);
					printf("%c",d);
					//SendData(d+1, XBEE);
				}
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
				XBeeRxState = STATE_IDLE;
			} break;
			
		}
	
		break;
	case FM:
		/*
		 * TODO: rajoutez le traitement a faire lors de la reception via FM 
		 */
		break;
	case GSM:
		/*
		 * TODO: rajoutez le traitement a faire lors de la reception via GSM 
		 */
		break;
	}
}							
