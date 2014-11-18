/*******************************************************************************
* File Name          : uart.c
* Author             : S DI Mercurio
* Version            : V5.0.0
* Date               : 15/07/2011
* Description        : focntion de manipualtion des USARTs
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

#include "mac.h"

void DataReceived (unsigned char d, ID_UART uart);

/*
 * void SendData(unsigned char d, ID_UART uart) 
 *
 * Envoi un caractere sur l'UART indiquée
 *
 * Parametres:
 *     Entrée:
 *	          d (unsigned char): caractere a envoyer	 
 *            uart (ID_UART): identifiant de la liaison serie	
 *     Sortie:
 *            rien
 *
 * Limitations:
 *     Attention, fonction bloquante le temps de l'envoi du caractere
 */
void SendData(unsigned char d, ID_UART uart) 
{
	/* Initialisation du pointeur sur le buffer de la liaison serie */
	switch (uart)
	{
		case XBEE:
			while ((USART2->SR & USART_SR_TC)!=USART_SR_TC);

			USART2->DR = d;
		
			while ((USART2->SR & USART_SR_TC)!=USART_SR_TC);
			break;
		case FM:
			while ((USART3->SR & USART_SR_TC)!=USART_SR_TC);

			USART3->DR = d;
			
			while ((USART3->SR & USART_SR_TC)!=USART_SR_TC);
			break;
		case GSM:
			while ((USART1->SR & USART_SR_TC)!=USART_SR_TC);

			USART1->DR = d;
		
			while ((USART1->SR & USART_SR_TC)!=USART_SR_TC);
			break;
	}
}

/*
 * void ActiverITRXXBEE (void)
 *
 * Active les interruptions en reception pour le XBEE
 *
 * Parametres:
 *     Entrée: 
 *            rien	
 *     Sortie:
 *            rien
 *
 * Limitations:
 *     Aucune
 */
void ActiverITRXXBEE (void)
{
volatile int tmp;

	tmp = USART2->DR;	
	NVIC_ClearPendingIRQ(USART2_IRQn);
	
	/* Enable USART2 Receive interrupts */
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

/*
 * void DesactiverITRXXBEE (void)
 *
 * Desactive les interruptions en reception pour le XBEE
 *
 * Parametres:
 *     Entrée: 
 *            rien	
 *     Sortie:
 *            rien
 *
 * Limitations:
 *     Aucune
 */
void DesactiverITRXXBEE (void)
{
	/* Disable USART2 Receive interrupts */
  USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
}

/*
 * void ActiverITRXFM (void)
 *
 * Active les interruptions en reception pour le XBEE
 *
 * Parametres:
 *     Entrée: 
 *            rien	
 *     Sortie:
 *            rien
 *
 * Limitations:
 *     Aucune
 */
void ActiverITRXFM (void)
{
volatile int tmp;

	tmp = USART3->DR;
	NVIC_ClearPendingIRQ(USART3_IRQn);
	
	/* Enable USART3 Receive interrupts */
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
}

/*
 * void DesactiverITRXFM (void)
 *
 * Desactive les interruptions en reception pour le XBEE
 *
 * Parametres:
 *     Entrée: 
 *            rien	
 *     Sortie:
 *            rien
 *
 * Limitations:
 *     Aucune
 */
void DesactiverITRXFM (void)
{
	/* Disable USART3 Receive interrupts */
  USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
}

/*
 * void ActiverITRXGSM (void)
 *
 * Active les interruptions en reception pour le XBEE
 *
 * Parametres:
 *     Entrée: 
 *            rien	
 *     Sortie:
 *            rien
 *
 * Limitations:
 *     Aucune
 */
void ActiverITRXGSM (void)
{
volatile int tmp;

	tmp = USART1->DR;
	NVIC_ClearPendingIRQ(USART1_IRQn);
	
	/* Enable USART1 Receive interrupts */
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

/*
 * void DesactiverITRXGSM (void)
 *
 * Desactive les interruptions en reception pour le XBEE
 *
 * Parametres:
 *     Entrée: 
 *            rien	
 *     Sortie:
 *            rien
 *
 * Limitations:
 *     Aucune
 */
void DesactiverITRXGSM (void)
{
	/* Disable USART1 Receive interrupts */
  USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
}
/*----------------------------------------------------------------------------
  USART1_IRQHandler -> GSM
  Handles USART1 global interrupt request.
 *----------------------------------------------------------------------------*/
void USART1_IRQHandler (void) 
{
int tmp;
volatile int dummy;

	tmp = USART1->DR;	/* Lecture de l'octet recu -> remise à zero du flag RXNE */

  	dummy = USART1->SR;
	dummy = USART1->CR1;
	dummy = USART1->CR2;
	dummy = USART1->CR3;
	dummy = USART1->BRR;
	dummy = USART1->SR;

  USART_ClearFlag(USART1, USART_FLAG_RXNE);  
  DataReceived(tmp, GSM); /* Envoi de l'octet à l'applicatif */
}

/*----------------------------------------------------------------------------
  USART2_IRQHandler
  Handles USART2 global interrupt request.
 *----------------------------------------------------------------------------*/
void USART2_IRQHandler (void) 
{
int tmp;
volatile int dummy;

	tmp = USART2->DR;	/* Lecture de l'octet recu -> remise à zero du flag RXNE */

	dummy = USART2->SR;
	dummy = USART2->CR1;
	dummy = USART2->CR2;
	dummy = USART2->CR3;
	dummy = USART2->BRR;
	dummy = USART2->SR;

  USART_ClearFlag(USART2, USART_FLAG_RXNE); 
  DataReceived(tmp, XBEE); /* Envoi de l'octet à l'applicatif */
}

/*----------------------------------------------------------------------------
  USART3_IRQHandler
  Handles USART3 global interrupt request.
 *----------------------------------------------------------------------------*/
void USART3_IRQHandler (void) 
{
int tmp;
volatile int dummy;

	tmp = USART3->DR;	/* Lecture de l'octet recu -> remise à zero du flag RXNE */

	dummy = USART3->SR;
	dummy = USART3->CR1;
	dummy = USART3->CR2;
	dummy = USART3->CR3;
	dummy = USART3->BRR;
	dummy = USART3->SR;

  USART_ClearFlag(USART3, USART_FLAG_RXNE); 
  DataReceived(tmp, FM); /* Envoi de l'octet à l'applicatif */
}

