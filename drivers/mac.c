/*******************************************************************************
* File Name          : mac.c
* Author             : S DI Mercurio puis J.L. Noullet
* Version            : V6b
* Date               : 27/10/2014
* Description        : Ce fichier fourni tout les services autres que ceux des uart
*                     
********************************************************************************
* Thoses drivers are free software; you can redistribute them and/or
* modify them under the terms of the GNU General Public
* License as published by the Free Software Foundation;
* either version 2, or (at your option) any later version.
*
* Thoses drivers are distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
* PURPOSE.  See the GNU General Public License for more
* details.
*******************************************************************************/
#include "stm32f10x.h"
#include "configuration.h"
#include "mac.h"

/******************************************************************************
 * 
 * Global
 *
 ******************************************************************************/

TYPE_NOEUD TypeNoeud=INDEFINI;
TYPE_CARTE TypeCarte=CARTE_INCONNUE;

/*
 * void MACInit(void)
 *
 * Initialisation de la couche MAC (GPIO, UART, Timer, IT, ...)
 *
 * Parametres:
 *     Entrée: 
 *            rien
 *
 *     Sortie:
 *            rien 
 *
 * Limitations:
 *     Aucune
 */
void MACInit(void)
{
	/* Init du micro et des peripheriques */
	Configuration();
	TurnOffFM();

	TIMEInit();

	if (GPIOC->IDR & (1<<10)) /* si PC10 == '1', la carte est une gateway, sinon une carte standard	*/
	{
		TypeCarte = GATEWAY;
	}
	else
	{
		TypeCarte = STANDARD;
	}

	if (TypeCarte == STANDARD) /* Init de l'ecran, si la carte est une carte standard */
	{
		lcd_init();	
		lcd_clear();
	}
}

/******************************************************************************
 * 
 * Module RT606
 *
 ******************************************************************************/

RT606_MODE etat_RT = RT606_OFF;

/*
 * void RS606SetMode(RS606_MODE mode)
 *
 * configure le module RT606 dans l'un de ses 4 modes possible
 *
 * Parametres:
 *     Entrée: 
 *            mode (RT606_MODE): mode de fonctionnement du RT606
 *						RT606_OFF = Le module est inactif
 *					    RT606_TX  = Le module est en emission 
 *						RT606_RX  = le module est en reception 
 *						RT606_TX_LOOPBACK = le module est en emission et reception: les données envoyées sont rebouclées en reception
 *
 *     Sortie:
 *            rien 
 *
 * Limitations:
 *     Aucune
 */
void RT606SetMode(RT606_MODE mode)
{
volatile int temp;

	if (mode != etat_RT) /* Inutile de modifier la config du module RT606 si c'est la meme que celle deja en cours */
	{
		/* Met les lignes PB9 (TX_CMD) PB8 (RX_CMD) en sortie  */
	
		/* remet PB8 et PB9 à 0 */
		GPIOB->BRR = GPIO_Pin_8 | GPIO_Pin_9;
	
		switch (mode)
		{
			case RT606_OFF:
				/* Rien a faire  */
				break;
			case RT606_TX:
				GPIOB->BSRR = GPIO_Pin_9; // met la ligne TX_CMD à 1 */ 
				break;
			case RT606_RX:
				GPIOB->BSRR = GPIO_Pin_8; // met la ligne RX_CMD à 1 */	
				break;
			case RT606_TX_LOOPBACK:
				GPIOB->BSRR = GPIO_Pin_8 | GPIO_Pin_9; /* Met les lignes TX_CMD et RX_CMD à 1 */
				break;
		}
	
		etat_RT = mode;
	}
}

/*
 * void TurnOnTXFM(void)
 *
 * configure le module RT606 en transmission (emission d'une porteuse)
 *
 * Parametres:
 *     Entrée: 
 *            rien
 *
 *     Sortie:
 *            rien 
 *
 * Limitations:
 *     Aucune
 */
void TurnOnTXFM(void)
{
	RT606SetMode(RT606_TX);
}

/*
 * void TurnOnRXFM(void)
 *
 * configure le module RT606 en reception (attention au bruit !!)
 *
 * Parametres:
 *     Entrée: 
 *            rien
 *
 *     Sortie:
 *            rien 
 *
 * Limitations:
 *     Aucune
 */
void TurnOnRXFM(void)
{
	RT606SetMode(RT606_RX);
}

/*
 * void TurnOffFM(void)
 *
 * Desactive le module RT606 (ni en reception, ni en transmission)
 *
 * Parametres:
 *     Entrée: 
 *            rien
 *
 *     Sortie:
 *            rien 
 *
 * Limitations:
 *     Aucune
 */
void TurnOffFM(void)
{
	RT606SetMode(RT606_OFF);
}

/*
 * CDSTATE_ENUM GetCDState(void)
 *
 * Lit l'etat de la porteuse FM
 *
 * Parametres:
 *     Entrée: 
 *            rien
 *
 *     Sortie:
 *            Etat de la porteuse 
 *
 * Limitations:
 *     Aucune
 */
CDSTATE_ENUM GetCDState(void)
{
CDSTATE_ENUM state=NOISE;

	if (etat_RT == RT606_OFF)
	{
		/* qq soit l'etat de CD, c'est du bruit*/
		state = NOISE;
	}

	if (etat_RT == RT606_TX)
	{
		/* qq soit l'etat de CD, c'est OK pour emettre */
		state = DATA_AVAILABLE;
	}

	if (etat_RT == RT606_RX)
	{
		if (GPIOB->IDR & GPIO_Pin_7)
		{
			/* Pas de CD -> Bruit */
			state = NOISE;
		}
		else
		{
			/* CD detectée -> Des données valables sont receptionables */
			state = DATA_AVAILABLE;
		}
	}

	return state;
}

/*
 * RT606_MODE RT606GetMode(void)
 *
 * Lit l'etat du module FM
 *
 * Parametres:
 *     Entrée: 
 *            rien
 *
 *     Sortie:
 *            Etat 
 *
 * Limitations:
 *     Aucune
 */
RT606_MODE RT606GetMode(void)
{
return etat_RT;
}

/******************************************************************************
 * 
 * Module GPIO
 *
 ******************************************************************************/

/*
 * Variables pour gerer l'antirebond (fonction GPIOButton)
 */

int GPIODebounceBuffer[2][2]; /* Tableau pour l'antirebond, à 2 lignes (pour les deux boutons)
                                 et 2 colonnes (pour les deux etats comparés) */
int GPIOButonLastState[2]= {BUTTON_RELEASED,BUTTON_RELEASED};    /* Tableau pour memoriser l'etat precedent des boutons et detecter les
                                 transitions appuyé->relaché et relaché->appuyé */
/*
 * int GPIOGetState (int port)
 *
 * Retourne l'etat courant d'un GPIO (sans antirebond)
 *
 * Parametres:
 *     Entrée: 
 *            port (int): identifiant du port
 *                	BUTTON_TAMP => pour lire l'etat du bouton TAMP
 *					BUTTON_WKUP => Pour lire l'etat du bouton WKUP 
 *     Sortie:
 *            int: etat du port (0 ou different de 0) ou -1 si l'identifiant du bouton est invalide 
 *
 * Limitations:
 *     Aucune
 */
int GPIOGetState (int port)
{
int state =0;

	switch (port)
	{
	case BUTTON_TAMP:
		state = (GPIOC->IDR & GPIO_Pin_13);
		break;
	case BUTTON_WKUP:
		state = (GPIOA->IDR & GPIO_Pin_0);
		break;
	default:
		state = -1;	
	}	

	return state;
}

/*
 * int GPIOButton (int button)
 *
 * Retourne l'etat d'un bouton (avec antirebond)
 * 
 * Parametres:
 *     Entrée: 
 *            button (int): identifiant du bouton
 *                	BUTTON_TAMP => pour lire l'etat du bouton TAMP
 *					BUTTON_WKUP => Pour lire l'etat du bouton WKUP 
 *     Sortie:
 *            int: etat du bouton (PRESSED, JUST_PRESSED, RELEASED, JUST_RELEASED) ou -1 si l'identifiant du bouton est invalide
 *
 * Limitations:
 *     Pour que l'antirebond fonctionne correctement, cette fonction ne doit pas etre appelée à une
 *     periode inferieur à 50ms.
 */
int GPIOButton (int button)
{
	/* Le dernier echantillon devient le plus ancien, et l'etat actuel du bouton est echantillonné */
	GPIODebounceBuffer[button][0] = GPIODebounceBuffer[button][1];
	GPIODebounceBuffer[button][1] = GPIOGetState(button);

	/* Si l'identifiant du bouton est invalide, renvoi -1 */
	if (GPIODebounceBuffer[button][1]==-1) return -1;
	 
	if (GPIODebounceBuffer[button][0] == GPIODebounceBuffer[button][1])	/* Le bouton est stable */
	{
		if (GPIOButonLastState[button] == GPIODebounceBuffer[button][1]) /* le bouton n'a pas changé d'etat depuis la derniere fois */
		{
			if (GPIODebounceBuffer[button][1] == 0) return BUTTON_PRESSED;
			else return BUTTON_RELEASED;
		}
		else /* Le bouton vient de changer d'etat */
		{
			GPIOButonLastState[button] = GPIODebounceBuffer[button][1]; /* Au memorise l'etat courant pour detecter un changement d'etat
			                                                               la prochaine fois */
			
			/* Transition bouton relaché-> appuyé */ 
			if (GPIODebounceBuffer[button][1]==0) return BUTTON_JUST_PRESSED;
			else return BUTTON_JUST_RELEASED;
		} 				
	}
	else 
	{
		if (GPIOButonLastState[button] == 0) return BUTTON_PRESSED; /* Le bouton n'est pas stable, on renvoi l'etat stable precedent */
		else return BUTTON_RELEASED;
	}

/* Suppression du warning qui indique que la fonction se termine sans return. ARMCC n'est pas trés futé sur ce coup là ... */
#pragma diag_remark	940
}

/******************************************************************************
 * 
 * Module CRC32 "soft"
 *
 ******************************************************************************/

#define CRC_POLY 0xEDB88320

/* ce module CRC soft utilise le polynome
     x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1.
   utilise par Ethernet, Zip, Rar, 7z et par le peripherique CRC du STM32.
   Ce module permet de calculer le CRC d'un nombre arbitraire de bytes,
   alors que le peripherique CRC du STM32 n'accepte qu'un multiple de 4 bytes.
   De plus l'ordre des bits ici est celui des CRC affiches par Zip, Rar, 7z
   alors que le peripherique CRC du STM32 fournit les bits de CRC dans l'ordre inverse (Ethernet).
   Utilisation :
   - creer un accumulateur de 32 bits, l'initialiser a 0xffffffff
   - appeler UpdateCRC( unsigned int * pcrc, unsigned char byte ) pour chaque byte
     du message
   - lire le CRC dans l'accumulateur
   - facultatif : complementer ce CRC si on veut le comparer a ce qui est fourni par Zip, Rar, 7z
 */  
/*
 * void UpdateCRC( unsigned int * pcrc, unsigned char data )
 *
 * met a jour le contenu de l'accumulateur de CRC (*pcrc)
 * en fonction de 8 bits de données data 
 *
 */
void UpdateCRC( unsigned int * pcrc, unsigned char data )
{
int i; unsigned char lsb;
for ( i = 0; i < 8; i++ )
    {
    lsb = (unsigned char)*pcrc;
    *pcrc >>= 1;
    if ( ( lsb ^ data ) & 1 )
       *pcrc ^= CRC_POLY;
    data >>= 1;
    }
}

/******************************************************************************
 * 
 * Module Timer
 *
 ******************************************************************************/

TIME_CALLBACK TIMECallback = 0;
int TIMEEnabled;
int TIME10msevent;

/*
 * void TIMEInit (void)
 *
 * Initialise le timer systeme qui va appeler une callback toute les 10ms
 * La fonction callback SysTick_Handler(void) est déjà prete, ci-dessous
 * Il est possible d'en definir une autre si necessaire (cf TIMESetCallback())
 *
 * Parametres:
 *     Entrée: 
 *            rien
 *     Sortie:
 *            Aucun
 *
 * Limitations:
 *     Aucune
 */
void TIMEInit (void)
{
	TIMEEnabled =1;
	TIME10msevent=0;
}

/*
 * void TIMESetCallback(TIME_CALLBACK callback)
 *
 * Accroche un callback à l'IT 10 ms du timer
 *
 * Parametres:
 *     Entrée: 
 *            callback (TIME_CALLBACK): un pointeur sur une fonction de type (void) (*TIME_CALLBACK)(void)
 *     Sortie:
 *            rien 
 *
 * Limitations:
 *     Aucune
 */
void TIMESetCallback(TIME_CALLBACK callback)
{
	TIMECallback = callback;
}

/*
 * int TIME10msExpired(void)
 *
 * Indique si une periode de 10ms s'est ecoulÃ©e sur le timer periodique
 *
 * Parametres:
 *     EntrÃ©e: 
 *            Aucun
 *     Sortie:
 *            int : vaut 1 si 10ms s'est ecoulÃ©e, 0 sinon 
 *
 * Limitations:
 *     Aucune
 */
int TIME10msExpired(void)
{ 
int status = FALSE;

	if (TIME10msevent == 1)
	{
		TIME10msevent =0;
		status = TRUE;
	}

	return status;
}

/*
 * void TIMEWait1ms(void)
 *
 * Cree un delai d'1 ms.
 *
 * Parametres:
 *     Entrée: 
 *            Aucun
 *     Sortie:
 *            Aucun 
 *
 * Limitations:
 *     Attention: fonction bloquante ! Ne pas utiliser sous interruption.
 */
void TIMEWait1ms(void)
{
#define _MAX_1MS_ 9000L

#ifndef _LCD_DEBUG_
volatile unsigned int i;
			   
	for (i=_MAX_1MS_; i!=0; i--); 
#endif /* _LCD_DEBUG_ */
}

/*
 * void TIMEWaitxms(void)
 *
 * Cree un delai de x ms.
 *
 * Parametres:
 *     Entrée: 
 *            ms (int): durée en ms.
 *     Sortie:
 *            Aucun 
 *
 * Limitations:
 *     Attention: fonction bloquante ! Ne pas utiliser sous interruption.
 */
void TIMEWaitxms(int ms)
{
volatile int cpt;

	for (cpt=0; cpt<ms; cpt++)
	{
		TIMEWait1ms();	
	}
}

/*
 * void SysTick_Handler(void)
 *
 * routine d'interruption du timer
 *
 * Parametres:
 *     Entrée: 
 *            Aucun
 *     Sortie:
 *            Aucun 
 *
 * Limitations:
 *     Aucune a par que c'est une routine d'interruption
 */
void SysTick_Handler(void)
{
	if (TIMEEnabled)
	{
		TIME10msevent=1;

		if (TIMECallback != 0x0)
		{
			TIMECallback();
		}
	}
}
