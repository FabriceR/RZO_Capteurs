/*******************************************************************************
* File Name          : configuration.c
* Author             : S DI Mercurio
* Version            : V6.0.0
* Date               : 15/07/2011
* Description        : Configuration des peripheriques
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

#include "clock.h"

void Configuration(void)
{
GPIO_InitTypeDef GPIO_InitStruct;
NVIC_InitTypeDef NVIC_InitStructure;
USART_InitTypeDef USART_InitStructure;
	
	/* Configuration de l'horloge */
	CLOCK_Configure();
	
	/* Activation des horloges GPIO A, B et C*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	/* Activation des horloges USART 1, 2 et 3 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	/* Configuration des IO */
	GPIO_DeInit(GPIOA);
	GPIO_DeInit(GPIOB);
	GPIO_DeInit(GPIOC);
	
	/* GPIOA */
	
	/* Pin 6, 7 et 8 (conn. JP 4, 5, 6 sur carte Std) en sortie push pull, 2Mhz */
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	/* Pin 2 et 9 (UARTs Tx) en sortie alternate push pull, 2Mhz */
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	/* Pin 3 et 10 (UARTs Rx) en entrée flottante */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	/* GPIOB */
	
	/* Pin 8 et 9 (cde FM) et 12, 13, 14, 15 (LEDs carte Keil) en sortie push pull, 2Mhz */
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Pin |= GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIOB->BRR = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;	// eteindre LEDs
	
	/* Pin 10 (UART Tx) en sortie alternate push pull, 2Mhz */
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	/* Pin 7 et 11 (FM CD et UART Rx) en entrées flottantes */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	/* GPIOC */
	/* Pin 12 (LED carte Olimex) en sortie push pull, 2Mhz */ 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	/* Pin 10 (auto-detect) en entrée flottante */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	/* Configuration des USART */
	USART_DeInit(USART1);
	USART_DeInit(USART2);
	USART_DeInit(USART3);
	
	/* USART 1: 115200 8/N/1 */
	USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	/* Configure USART1 */
  USART_Init(USART1, &USART_InitStructure);

  /* Enable the USART1 */
  USART_Cmd(USART1, ENABLE);
	
	/* USART 2: 9600 8/N/1 */
	USART_InitStructure.USART_BaudRate = 9600;
	
	/* Configure USART2 */
  USART_Init(USART2, &USART_InitStructure);

  /* Enable the USART2 */
  USART_Cmd(USART2, ENABLE);
	
	/* USART 3: 9600 8/N/1 */
	USART_InitStructure.USART_BaudRate = 9600;
	
	/* Configure USART3 */
  USART_Init(USART3, &USART_InitStructure);

  /* Enable the USART3 */
  USART_Cmd(USART3, ENABLE);
	
	/* Configuration des IT */
	/* Configure la repartition de priorité du NVIC (priortyGrouping) 
	   PriorityGroup_4 => 15 niveaux de preemption, 0 niveau de subgrouping */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //Fonction fournie dans misc.c
	
	/* Enable the USART1 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
	
	/* Enable the USART2 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Enable the USART3 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Configure la priorité de l'interruption Systick pour une priorité de 15 (la plus faible)*/
	NVIC_SetPriority(SysTick_IRQn,15); // Fonction fournie dans core_cm3.h
	
	/* Configuration du systick pour une interruption toutes les 10 ms
	   Comme la frequence du coeur est maintenant de 72 Mhz, le systick etant sur 24 bits, 
		 il ne peut plus fournir une base de temps suffisament grande.
		 
		 On le divise par 8 dans ce cas */
	SysTick_Config((72000000UL/8UL)/100UL); 	// Configure le systick pour une interruption toutes les 10 ms
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	// On regle la division du systick apres la fonction Systick_Config
	                                                      // car Systick_Config regle toujours la division à 1

#if !defined _LCD_DEBUG_
	*((uint32_t*)0xE0042004)=0x00000027;	/* Activation de la trace */
#endif
}
